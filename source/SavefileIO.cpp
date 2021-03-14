#include "SavefileIO.h"

#include <fstream>
#include <dirent.h>
#include <iostream>
#include <fcntl.h>   // open
#include <unistd.h>  // read, write, close
#include <cstdio>    // BUFSIZ
#include <thread>

#include <iostream>
#include <iomanip>
#include <stdio.h> 
#include <unistd.h>

#include <switch.h>

#include "Accounts.h"

uint32_t SavefileIO::ReadU32(unsigned char* buffer, int offset)
{
	// Little endian byte order
	return  ((buffer[offset + 0] << 0 ) + 
			(buffer[offset + 1] << 8 ) + 
			(buffer[offset + 2] << 16) + 
			(buffer[offset + 3] << 24)) >> 0 /* Make it positive? */;
}

bool SavefileIO::MountSavefile()
{
    Result rc=0; 

    AccountUid uid={0};
    u64 application_id=0x01007ef00011e000; //ApplicationId of the save to mount, in this case BOTW.

    //Get the userID for save mounting. To mount common savedata, use an all-zero userID.

    // Display the profile picker so the user can choose the profile
    uid = Accounts::RequestProfileSelection();

    std::cout << "User uid: " << uid.uid[0] << ", " << uid.uid[1] << "\n";

    // Check if the user canceled the dialog (if so, the uid is not valid)
    if (!accountUidIsValid(&uid))
    {
        printf("The user canceled the profile picker (or an error occurred)\n");
        return false;
    }

    //You can use any device-name. If you want multiple saves mounted at the same time, you must use different device-names for each one.
    rc = fsdevMountSaveData("save", application_id, uid); //See also libnx fs.h/fs_dev.h
    if (R_FAILED(rc)) {
        printf("fsdevMountSaveData() failed: 0x%x\n", rc);
        return false;
    }

    AccountUid1 = uid.uid[0];
    AccountUid2 = uid.uid[1];

    return true;
}

bool SavefileIO::UnmountSavefile()
{
    //When you are done with savedata, you can use the below.
    //Any devices still mounted at app exit are automatically unmounted.
    return R_SUCCEEDED(fsdevUnmountDevice("save"));
}

void SavefileIO::CopySavefiles(u64 uid1, u64 uid2, bool* doneFlag)
{
    printf("Started copy thread\n");

    std::vector<std::string> savefileFolders;

    DIR* dir = opendir("save:/");
    if(dir==NULL)
    {
        printf("Couldn't copy savefiles. Failed to open 'save:/'\n");

        *doneFlag = true;

        return;
    }
    else
    {
        // Normal mode file
        if (DirectoryExists("save:/0"))
            savefileFolders.push_back("0");
        // Master mode file
        if (DirectoryExists("save:/7"))
            savefileFolders.push_back("7");
        // Prioritize file 7, but copy file 6 if 7 doesn't exist
        else if (DirectoryExists("save:/6"))
            savefileFolders.push_back("6");

        closedir(dir);
    }

    std::string profileIdStr = std::to_string(uid1) + " - " + std::to_string(uid2);
    std::string savesFolder = "sdmc:/switch/botw-unexplored/saves/" + profileIdStr;

    if (!DirectoryExists("sdmc:/switch/botw-unexplored"))
        mkdir("sdmc:/switch/botw-unexplored", 0777);
    if (!DirectoryExists("sdmc:/switch/botw-unexplored/saves"))
        mkdir("sdmc:/switch/botw-unexplored/saves", 0777);
    if (!DirectoryExists(savesFolder))
        mkdir(savesFolder.c_str(), 0777);

    for (unsigned int i = 0; i < savefileFolders.size(); i++)
    {
        // Create the destination folder if it doesn't exist
        if (!DirectoryExists(savesFolder + "/" + savefileFolders[i]))
            mkdir(std::string(savesFolder + "/" + savefileFolders[i]).c_str(), 0777);

        // Copy the savefiles
        std::string target = savesFolder + "/" + savefileFolders[i] + "/game_data.sav";

        CopyFile("save:/" + savefileFolders[i] + "/game_data.sav", target);

        std::cout << "Copied savefile " << savefileFolders[i] << " to " << target << "\n";
    }

    SavefileIO::UnmountSavefile();

    *doneFlag = true;

    printf("Finished copying savefiles\n");
}

bool SavefileIO::CopyFile(const std::string& file, const std::string& destination)
{
    size_t BUFFER_SIZE = 4096;

    char buf[BUFFER_SIZE];
    size_t size;

    int source = open(file.c_str(), O_RDONLY, 0);
    int dest = open(destination.c_str(), O_WRONLY | O_CREAT /*| O_TRUNC*/, 0644);

    while ((size = read(source, buf, BUFSIZ)) > 0) {
        write(dest, buf, size);
    }

    close(source);
    close(dest);

    return true;
}

bool SavefileIO::DirectoryExists(const std::string& filepath)
{
    DIR* dir = opendir(filepath.c_str());
    if (dir) {
        closedir(dir);

        return true;
    } else if (ENOENT == errno)
        return false;
    else {
        std::cout << "DirectoryExists() " << filepath << "\n";
        return false;
    }

    return false;
}

bool SavefileIO::ParseFile(const char* filepath)
{
    printf("Opening savefile '%s'\n", filepath);

    int res = access(filepath, R_OK);
    if (res < 0) {
        if (errno == ENOENT)
            printf("Savefile '%s' could not be opened\n", filepath);
        else if (errno == EACCES)
            printf("Savefile '%s' could not be read for some reason\n", filepath);
        else 
            printf("Savefile '%s' failed\n", filepath);

        return false;
    }

    std::ifstream file;
	file.open(filepath, std::ios::binary);

	// Get length of file
	file.seekg(0, file.end);
	unsigned int fileSize = (unsigned int) file.tellg(); // File size is below sizeof(unsigned int), so the cast is fine
	file.seekg(0, file.beg);

	unsigned char* buffer = new unsigned char[fileSize];

	// Read the entire file into the buffer. Need to cast the buffer to a non-signed char*.
	file.read((char*) &buffer[0], fileSize);

	file.close();

	// Based on https://github.com/d4mation/botw-unexplored-viewer/blob/master/assets/js/zelda-botw.js

	// Iterate through the entire savefile to find the korok seed and location hashes
	for (unsigned int offset = 0x0c; offset < fileSize - 4; offset += 8)
	{
		// Read the korok or location hash ('id')
		uint32_t hashValue = ReadU32(buffer, offset);

		// Check if a korok with the hash exists
		Data::Korok* korok = Data::KorokExists(hashValue);
		if (korok)
		{
			// Read the 4 bytes after the hash. If it's not 0, then the seed has been found.
			bool found = ReadU32(buffer, offset + 4) != 0;

			found ? foundKoroks.push_back(korok) : missingKoroks.push_back(korok);
		}

		// Check if a location with the hash exists
		Data::Location* location = Data::LocationExists(hashValue);
		if (location)
		{
			// Read the 4 bytes after the hash. If it is not 0, then the location has been visited.
			bool visited = ReadU32(buffer, offset + 4) != 0;

			visited ? visitedLocations.push_back(location) : unexploredLocations.push_back(location);
		}

		// Check if a hinox with the hash exists
		Data::Hinox* hinox = Data::HinoxExists(hashValue);
		if (hinox)
		{
			// Read the 4 bytes after the hash. If it is not 0, then the hinox has been defeated.
			bool defeated = ReadU32(buffer, offset + 4) != 0;

			defeated ? defeatedHinoxes.push_back(hinox) : undefeatedHinoxes.push_back(hinox);
		}

		// Check if a talus with the hash exists
		Data::Talus* talus = Data::TalusExists(hashValue);
		if (talus)
		{
			// Read the 4 bytes after the hash. If it is not 0, then the talus has been defeated.
			bool defeated = ReadU32(buffer, offset + 4) != 0;

			defeated ? defeatedTaluses.push_back(talus) : undefeatedTaluses.push_back(talus);
		}

		// Check if a molduga with the hash exists
		Data::Molduga* molduga = Data::MoldugaExists(hashValue);
		if (molduga)
		{
			// Read the 4 bytes after the hash. If it is not 0, then the molduga has been defeated.
			bool defeated = ReadU32(buffer, offset + 4) != 0;

			defeated ? defeatedMoldugas.push_back(molduga) : undefeatedMoldugas.push_back(molduga);
		}
	}

    printf("Successfully parsed file '%s'\n", filepath);

    return true;
}

std::vector<Data::Korok*> SavefileIO::foundKoroks;
std::vector<Data::Korok*> SavefileIO::missingKoroks;

std::vector<Data::Location*> SavefileIO::visitedLocations;
std::vector<Data::Location*> SavefileIO::unexploredLocations;

std::vector<Data::Hinox*> SavefileIO::defeatedHinoxes;
std::vector<Data::Hinox*> SavefileIO::undefeatedHinoxes;

std::vector<Data::Talus*> SavefileIO::defeatedTaluses;
std::vector<Data::Talus*> SavefileIO::undefeatedTaluses;

std::vector<Data::Molduga*> SavefileIO::defeatedMoldugas;
std::vector<Data::Molduga*> SavefileIO::undefeatedMoldugas;

u64 SavefileIO::AccountUid1;
u64 SavefileIO::AccountUid2;