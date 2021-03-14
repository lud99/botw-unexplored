#include "SavefileIO.h"

#include <fstream>
#include <dirent.h>

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

    //At this point you can use the mounted device with standard stdio.
    //After modifying savedata, in order for the changes to take affect you must use: rc = fsdevCommitDevice("save");
    //See also libnx fs_dev.h for fsdevCommitDevice.

    DIR* dir = opendir("save:/"); //Open the "save:/" directory.
    struct dirent* ent;
    if(dir==NULL)
    {
        printf("Failed to open save:/.\n");

        return false;
    }
    else
    {
        printf("Dir-listing for 'save:/':\n");
        while ((ent = readdir(dir)))
        {
            printf("%s\n", ent->d_name);
        }
        closedir(dir);
    }

    return true;
}

bool SavefileIO::UnmountSavefile()
{
    //When you are done with savedata, you can use the below.
    //Any devices still mounted at app exit are automatically unmounted.
    return R_SUCCEEDED(fsdevUnmountDevice("save"));
}

bool SavefileIO::ParseFile(const char* filepath)
{
    printf("Opening savefile '%s'...\n", filepath);

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

    printf("Parsing savefile '%s'...\n", filepath);

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