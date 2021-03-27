#include "SavefileIO.h"

#include <fstream>
#include <dirent.h>
#include <iostream>
#include <map>
#include <iostream>
#include <stdio.h>

#include <switch.h>

#include "Accounts.h"
#include "Map.h"
#include "Dialog.h"
#include "Log.h"

bool SavefileIO::LoadGamesave(bool loadMasterMode, bool chooseProfile)
{
    Log("Loading gamesave. Master mode: " + std::string(loadMasterMode ? "true" : "false") + 
       ", Choose profile (instead of automatic): " + std::string(chooseProfile ? "true" : "false"));

    // Try to mount the save directory
    int mountStatus = MountSavefile(chooseProfile);

    bool dialogWasOpen = Map::m_GameRunningDialog->m_IsOpen || Map::m_MasterModeDialog->m_IsOpen || Map::m_NoSavefileDialog->m_IsOpen;

    Map::m_GameRunningDialog->SetOpen(false);
    Map::m_MasterModeDialog->SetOpen(false);
    Map::m_NoSavefileDialog->SetOpen(false);

    Log("Mount status:", mountStatus);

    if (mountStatus == 1) { // Good
        if (loadMasterMode)
        {
            bool success = false;
            if (MasterModeFileExists) 
            {
                std::string path = "save:/" + std::to_string(MostRecentMasterModeFile) + "/game_data.sav";
                success = ParseFile(path.c_str());
                if (!success)
                    Log("Mastermode savefile failed to parse");
            }

            if (success) MasterModeFileLoaded = true;

            UnmountSavefile();
                
            return success;
        } else
            MasterModeFileLoaded = false;

        std::string path = "save:/" + std::to_string(MostRecentNormalModeFile) + "/game_data.sav";
        bool success = ParseFile(path.c_str());
        if (!success) 
        {
            Log("Normal mode savefile failed to parse");

            UnmountSavefile();
            Map::m_NoSavefileDialog->SetOpen(true);
            return false;
        }

        // No need to copy savefiles if they have already been copied (this flag is never set the first time)
        if (!chooseProfile)
            CopySavefiles();

        UnmountSavefile();
    } 
    // Failed to mount it. Can happen if no profile was choosen, or some other account thing went wrong 
    else if (mountStatus == 0) {  // No save for profile
        Log("Canceled profile picker");

        return false;
    } else if (mountStatus == -1) { // Game is running
        Log("Game is running. Loading backup...");

        bool loadedBackupSuccess = LoadBackup(loadMasterMode);

        if (!loadedBackupSuccess) 
        {
            LoadedSavefile = false;
            MasterModeFileLoaded = false;
            Map::m_GameRunningDialog->SetOpen(true);

            return false;
        }

        // if (dialogWasOpen)
        //     Map::m_Legend->m_IsOpen = true;
    } else if (mountStatus == -2) { // User has no save data
        Log("The selected user has no save data");

        MasterModeFileLoaded = false;
        Map::m_NoSavefileDialog->SetOpen(true);
        
        UnmountSavefile();

        return false;
    }

    return true;
}

uint32_t SavefileIO::ReadU32(unsigned char *buffer, int offset)
{
    // Little endian byte order
    return ((buffer[offset + 0] << 0) +
            (buffer[offset + 1] << 8) +
            (buffer[offset + 2] << 16) +
            (buffer[offset + 3] << 24)) >>
           0 /* Make it positive? */;
}

int SavefileIO::MountSavefile(bool openProfilePicker)
{
    Result rc = 0;
    u64 botwId = 0x01007ef00011e000;
    AccountUid uid = {0};

    MasterModeFileExists = false;   
    GameIsRunning = false;
    LoadedSavefile = false;

    if (openProfilePicker)
        uid = Accounts::RequestProfileSelection();
    else
    {
        // Use cached values
        uid.uid[0] = AccountUid1;
        uid.uid[1] = AccountUid2;

        if (accountUidIsValid(&uid))
        {
            Result rc = fsdevMountSaveData("save", botwId, uid);
            if (R_FAILED(rc))
            {
                Log("Failed to mount save (cached user)");
                GameIsRunning = true;

                return -1;
            }

            Log("Using cached account");

            // Figure out which save file is the most recent one
            MostRecentNormalModeFile = GetMostRecentSavefile("save:/", false);
            MostRecentMasterModeFile = GetMostRecentSavefile("save:/", true);

            if (MostRecentNormalModeFile == -1) 
            {
                Log("No normal mode file exists (cached user)");
                return -2;
            }

            return 1;
        } else {
            Log("No cached profile available");
        }
    }
    
    // Required for getting users
    rc = accountInitialize(AccountServiceType_Administrator);
    if (R_FAILED(rc)) {
        Log("accountInitialize() failed");
    }

    // If the manual profile selection wasn't choosen 
    if (!accountUidIsValid(&uid))
    {
        rc = accountGetLastOpenedUser(&uid);
        bool couldGetUserAutomatically = false;

        if (R_SUCCEEDED(rc)) {
            // Check if there is a user that last opened an app (ex. if you have restarted the console)
            couldGetUserAutomatically = accountUidIsValid(&uid);
            if (!couldGetUserAutomatically) 
            {
                Log("No last user available.");
                
                // Try to get the user that was used to launch the app when holding down R
                rc = accountGetPreselectedUser(&uid);
                if (R_FAILED(rc))
                {
                    couldGetUserAutomatically = false;
                    Log("No user used to launch the app");
                } else if (R_SUCCEEDED(rc) && accountUidIsValid(&uid))
                {
                    couldGetUserAutomatically = true;
                    Log("Got user used to launch the app");
                }
            } else {
                Log("Using last user used to launch app");
            }
        }

        if (!couldGetUserAutomatically) 
        {
            Log("Opening profile picker");

            uid = Accounts::RequestProfileSelection();
        }
    }

    accountExit();

    AccountUid1 = uid.uid[0];
    AccountUid2 = uid.uid[1];

    // Check if the user canceled the dialog (if so, the uid is not valid)
    if (!accountUidIsValid(&uid))
    {
        Log("Invalid account uid. The user canceled the profile picker");
        return -2;
    } else {
        Log("Valid account uid from whatever profile selection method");
    }

    FsSaveDataInfoReader reader;
    FsSaveDataInfo info;
    s64 total_entries = 0;

    bool hasBotwSavedata = false;

    Result res = fsOpenSaveDataInfoReader(&reader, FsSaveDataSpaceId_User);
    if (R_FAILED(res)) {
        Log("fsOpenSaveDataInfoReader() failed");
        return -2;
    }

    while (true) {
        res = fsSaveDataInfoReaderRead(&reader, &info, 1, &total_entries);
        if (R_FAILED(res) || total_entries == 0) {
            break;
        }

        if (info.save_data_type == FsSaveDataType_Account) {
            // Check if the save is botw and belongs to the selected user
            if (info.application_id == botwId && info.uid.uid[0] == uid.uid[0] && info.uid.uid[1] == uid.uid[1])
            {
                hasBotwSavedata = true;
                Log("User has botw savedata");
                break;
            }
        }
    }

    fsSaveDataInfoReaderClose(&reader);

    if (!hasBotwSavedata)
    {
        Log("User has no botw save data");
        NoSavefileForUser = true;
        return -2;
    }

    rc = fsdevMountSaveDataReadOnly("save", botwId, uid);
    if (R_FAILED(rc))
    {
        Log("Botw is running. Failed to mount save (or fsdevMountSaveDataReadOnly() failed, who knows)");
        GameIsRunning = true;

        return -1;
    }

    // Figure out which save file is the most recent one
    MostRecentNormalModeFile = GetMostRecentSavefile("save:/", false);
    MostRecentMasterModeFile = GetMostRecentSavefile("save:/", true);

    if (MostRecentNormalModeFile == -1)
    {
        Log("No savefile exists, even though savedata exists");
        return -2;
    }

    return 1;
}

bool SavefileIO::UnmountSavefile()
{
    //When you are done with savedata, you can use the below.
    //Any devices still mounted at app exit are automatically unmounted.
    bool fail = R_FAILED(fsdevUnmountDevice("save"));
    if (fail)
    {
        Log("Failed to unmount save");
        return true;
    }

    return true;
}

bool SavefileIO::LoadBackup(bool masterMode)
{
    std::string profileIdStr = std::to_string(AccountUid1) + " - " + std::to_string(AccountUid2);
    std::string savesFolder = "sdmc:/switch/botw-unexplored/saves/" + profileIdStr + "/";

    if (!DirectoryExists("sdmc:/switch/botw-unexplored"))
        return false;
    if (!DirectoryExists("sdmc:/switch/botw-unexplored/saves"))
        return false;
    if (!DirectoryExists(savesFolder))
    {
        Log("No backups exist for this user");
        return false;
    }

    // Get most recent savefile
    MostRecentNormalModeFile = GetMostRecentSavefile(savesFolder, false);
    MostRecentMasterModeFile = GetMostRecentSavefile(savesFolder, true);

    // So savefile exists
    if (MostRecentNormalModeFile == -1)
        return false;
    if (MostRecentMasterModeFile == -1 && masterMode)
        return false;

    std::string savefilePath = savesFolder + std::to_string(!masterMode ? MostRecentNormalModeFile : MostRecentMasterModeFile) + "/game_data.sav";

    // Parse it
    return ParseFile(savefilePath.c_str());
}

uint32_t SavefileIO::GetSavefilePlaytime(const std::string& filepath)
{
    if (!FileExists(filepath))
        return 0;

    std::ifstream file;
    file.open(filepath, std::ios::binary);

    // Get length of file
    file.seekg(0, file.end);
    unsigned int fileSize = (unsigned int)file.tellg(); // Get file size
    file.seekg(0, file.beg);

    unsigned char *buffer = new unsigned char[fileSize];

    // Read the entire file into the buffer. Need to cast the buffer to a non-signed char*.
    file.read((char *)&buffer[0], fileSize);

    file.close();

    uint32_t playtimeHash = 0x73c29681;
    
    // Iterate to find the location of the hash
    uint32_t playtime = 0;
    for (unsigned int offset = 0x0c; offset < fileSize - 4; offset += 8)
    {
        // Read the hash
        uint32_t hashValue = ReadU32(buffer, offset);
        if (hashValue == playtimeHash)
        {
            playtime = ReadU32(buffer, offset + 4);
            break;
        }
    }

    delete buffer;

    return playtime;
}

int SavefileIO::GetMostRecentSavefile(const std::string& dir, bool masterMode)
{
    // Figure out which save file is the most recent one

    uint32_t highestPlaytime = 0;
    int mostRecentFile = -1;

    // Normal mode
    if (!masterMode)
    {
        for (int i = 0; i <= 5; i++)
        {
            std::string path = dir + std::to_string(i) + "/game_data.sav";
            uint32_t playtime = GetSavefilePlaytime(path); // 0 = Save doesn't exist

            // If this file is more recent then the currently highest one
            if (playtime != 0 && playtime >= highestPlaytime) 
            {
                highestPlaytime = playtime;
                mostRecentFile = i;
            }
        }

        return mostRecentFile;
    }

    // Master mode
    for (int i = 6; i <= 7; i++)
    {
        std::string path = dir + std::to_string(i) + "/game_data.sav";
        uint32_t playtime = GetSavefilePlaytime(path); // 0 = Save doesn't exist

        // If this file is more recent then the currently highest one
        if (playtime != 0 && playtime >= highestPlaytime) 
        {
            highestPlaytime = playtime;
            mostRecentFile = i;
        }
    }

    if (mostRecentFile != -1)
        MasterModeFileExists = true;

    return mostRecentFile;
}

void SavefileIO::CopySavefiles()
{
    Log("Copying savefiles...");
    std::vector<std::string> savefileFolders;

    if (MostRecentNormalModeFile != -1)
        savefileFolders.push_back(std::to_string(MostRecentNormalModeFile));
    if (MostRecentMasterModeFile != -1) 
        savefileFolders.push_back(std::to_string(MostRecentMasterModeFile));

    std::string profileIdStr = std::to_string(AccountUid1) + " - " + std::to_string(AccountUid2);
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

        Log("Copied savefile " + savefileFolders[i] + " to " + target);
    }

    SavefileIO::UnmountSavefile();
}

s32 SavefileIO::CopyFile(const std::string &srcPath, const std::string &dstPath)
{
    FILE *src = fopen(srcPath.c_str(), "rb");
    FILE *dst = fopen(dstPath.c_str(), "wb+");

    if (src == nullptr || dst == nullptr)
        return -1;

    fseek(src, 0, SEEK_END);
    rewind(src);

    size_t size;
    char *buf = new char[0x50000];

    u64 offset = 0;
    size_t slashpos = srcPath.rfind("/");
    std::string name = srcPath.substr(slashpos + 1, srcPath.length() - slashpos - 1);
    while ((size = fread(buf, 1, 0x50000, src)) > 0)
    {
        fwrite(buf, 1, size, dst);
        offset += size;
    }

    delete[] buf;
    fclose(src);
    fclose(dst);

    return 0;
}

bool SavefileIO::DirectoryExists(const std::string &filepath)
{
    DIR *dir = opendir(filepath.c_str());
    if (dir)
    {
        closedir(dir);

        return true;
    }
    else if (ENOENT == errno)
        return false;
    else
    {
        Log("DirectoryExists() error ", filepath);
        return false;
    }

    return false;
}

bool SavefileIO::FileExists(const std::string& filepath)
{
    int res = access(filepath.c_str(), R_OK);
    if (res < 0)
    {
        if (errno == ENOENT)
            Log("File doesn't exist (doesn't have to be bad)", filepath.c_str());
        else if (errno == EACCES)
            Log("File could not be read for some reason (doesn't have to be bad)", filepath.c_str());
        else
            Log("File access failed (doesn't have to be bad)", filepath.c_str());

        return false;
    }

    return true;
}

bool SavefileIO::ParseFile(const char *filepath)
{
    Log("Opening savefile", filepath);

    // Clear the current file data
    foundKoroks.clear();
    missingKoroks.clear();
    foundShrines.clear();
    missingShrines.clear();
    foundDLCShrines.clear();
    missingDLCShrines.clear();
    visitedLocations.clear();
    unexploredLocations.clear();
    defeatedHinoxes.clear();
    undefeatedHinoxes.clear();
    defeatedTaluses.clear();
    undefeatedTaluses.clear();
    defeatedMoldugas.clear();
    undefeatedMoldugas.clear();

    if (!FileExists(filepath))
        return false;

    std::ifstream file;
    file.open(filepath, std::ios::binary);

    // Get length of file
    file.seekg(0, file.end);
    unsigned int fileSize = (unsigned int)file.tellg(); // File size is below sizeof(unsigned int), so the cast is fine
    file.seekg(0, file.beg);

    unsigned char *buffer = new unsigned char[fileSize];

    // Read the entire file into the buffer. Need to cast the buffer to a non-signed char*.
    file.read((char *)&buffer[0], fileSize);

    file.close();

    // Based on https://github.com/d4mation/botw-unexplored-viewer/blob/master/assets/js/zelda-botw.js

    // Iterate through the entire savefile to find the korok seed and location hashes
    for (unsigned int offset = 0x0c; offset < fileSize - 4; offset += 8)
    {
        // Read the korok or location hash ('id')
        uint32_t hashValue = ReadU32(buffer, offset);

        // Check if a korok with the hash exists
        Data::Korok *korok = Data::KorokExists(hashValue);
        if (korok)
        {
            // Read the 4 bytes after the hash. If it's not 0, then the seed has been found.
            bool found = ReadU32(buffer, offset + 4) != 0;

            found ? foundKoroks.push_back(korok) : missingKoroks.push_back(korok);
        }
        

        // Check if a shrine with the hash exists
        Data::Shrine *shrine = Data::ShrineExists(hashValue);
        if (shrine)
        {
            // Read the 4 bytes after the hash. If it is not 0, then the shrine has been found.
            bool defeated = ReadU32(buffer, offset + 4) != 0;

            defeated ? foundShrines.push_back(shrine) : missingShrines.push_back(shrine);
        }

        // Check for dlc shrines
        Data::DLCShrine *dlcShrine = Data::DLCShrineExists(hashValue);
        if (dlcShrine)
        {
            // Read the 4 bytes after the hash. If it is not 0, then the shrine has been found.
            bool defeated = ReadU32(buffer, offset + 4) != 0;

            defeated ? foundDLCShrines.push_back(dlcShrine) : missingDLCShrines.push_back(dlcShrine);
        }

        // Check if a location with the hash exists
        Data::Location *location = Data::LocationExists(hashValue);
        if (location)
        {
            // Read the 4 bytes after the hash. If it is not 0, then the location has been visited.
            bool visited = ReadU32(buffer, offset + 4) != 0;

            visited ? visitedLocations.push_back(location) : unexploredLocations.push_back(location);
        }

        // Check if a hinox with the hash exists
        Data::Hinox *hinox = Data::HinoxExists(hashValue);
        if (hinox)
        {
            // Read the 4 bytes after the hash. If it is not 0, then the hinox has been defeated.
            bool defeated = ReadU32(buffer, offset + 4) != 0;

            defeated ? defeatedHinoxes.push_back(hinox) : undefeatedHinoxes.push_back(hinox);
        }

        // Check if a talus with the hash exists
        Data::Talus *talus = Data::TalusExists(hashValue);
        if (talus)
        {
            // Read the 4 bytes after the hash. If it is not 0, then the talus has been defeated.
            bool defeated = ReadU32(buffer, offset + 4) != 0;

            defeated ? defeatedTaluses.push_back(talus) : undefeatedTaluses.push_back(talus);
        }

        // Check if a molduga with the hash exists
        Data::Molduga *molduga = Data::MoldugaExists(hashValue);
        if (molduga)
        {
            // Read the 4 bytes after the hash. If it is not 0, then the molduga has been defeated.
            bool defeated = ReadU32(buffer, offset + 4) != 0;

            defeated ? defeatedMoldugas.push_back(molduga) : undefeatedMoldugas.push_back(molduga);
        }

        // Check if has the dlc
        uint32_t BalladOfHeroes_Ready = 1186840637; // Set to true if the DLC is owned
        if (hashValue == BalladOfHeroes_Ready) {
            HasDLC = ReadU32(buffer, offset + 4) == 1;
            Log("User has DLC:", HasDLC ? "true" : "false");
        }
    }

    delete buffer;

    Log("Successfully parsed file", filepath);

    LoadedSavefile = true;

    return true;
}

std::vector<Data::Korok *> SavefileIO::foundKoroks;
std::vector<Data::Korok *> SavefileIO::missingKoroks;
std::vector<Data::Shrine*> SavefileIO::foundShrines;
std::vector<Data::Shrine*> SavefileIO::missingShrines;
std::vector<Data::DLCShrine*> SavefileIO::foundDLCShrines;
std::vector<Data::DLCShrine*> SavefileIO::missingDLCShrines;
std::vector<Data::Location *> SavefileIO::visitedLocations;
std::vector<Data::Location *> SavefileIO::unexploredLocations;
std::vector<Data::Hinox *> SavefileIO::defeatedHinoxes;
std::vector<Data::Hinox *> SavefileIO::undefeatedHinoxes;
std::vector<Data::Talus *> SavefileIO::defeatedTaluses;
std::vector<Data::Talus *> SavefileIO::undefeatedTaluses;
std::vector<Data::Molduga *> SavefileIO::defeatedMoldugas;
std::vector<Data::Molduga *> SavefileIO::undefeatedMoldugas;

u64 SavefileIO::AccountUid1;
u64 SavefileIO::AccountUid2;
int SavefileIO::MostRecentNormalModeFile = -1;
int SavefileIO::MostRecentMasterModeFile = -1;
bool SavefileIO::LoadedSavefile = false;
bool SavefileIO::GameIsRunning = false;
bool SavefileIO::NoSavefileForUser = false;
bool SavefileIO::MasterModeFileExists = false;
bool SavefileIO::MasterModeFileLoaded = false;
bool SavefileIO::HasDLC = false;

int SavefileIO::MasterModeSlot;