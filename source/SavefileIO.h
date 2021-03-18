#pragma once

#include <vector>
#include <cstdint>
#include <switch.h>
#include <thread>

#include "Data.h"

namespace SavefileIO
{
    extern std::vector<Data::Korok*> foundKoroks;
    extern std::vector<Data::Korok*> missingKoroks;

    extern std::vector<Data::Shrine*> foundShrines;
    extern std::vector<Data::Shrine*> missingShrines;

    extern std::vector<Data::Location*> visitedLocations;
    extern std::vector<Data::Location*> unexploredLocations;

    extern std::vector<Data::Hinox*> defeatedHinoxes;
    extern std::vector<Data::Hinox*> undefeatedHinoxes;

    extern std::vector<Data::Talus*> defeatedTaluses;
    extern std::vector<Data::Talus*> undefeatedTaluses;

    extern std::vector<Data::Molduga*> defeatedMoldugas;
    extern std::vector<Data::Molduga*> undefeatedMoldugas;

    uint32_t ReadU32(unsigned char* buffer, int offset);

    int MountSavefile(bool openProfilePicker = false);
    bool UnmountSavefile();

    bool LoadBackup(bool masterMode = false);

    uint32_t GetSavefilePlaytime(const std::string& filepath);
    int GetMostRecentSavefile(const std::string& dir, bool masterMode = false); // Needs slash at end of dir

    void CopySavefiles();
    s32 CopyFile(const std::string &srcPath, const std::string &dstPath);

    bool ParseFile(const char* filepath);

    bool DirectoryExists(const std::string& filepath);
    bool FileExists(const std::string& filepath);

    extern u64 AccountUid1;
    extern u64 AccountUid2;

    extern int MostRecentNormalModeFile;
    extern int MostRecentMasterModeFile;

    extern bool LoadedSavefile;
    extern bool GameIsRunning;
    extern bool NoSavefileForUser;
    extern bool MasterModeFileExists;
    extern bool MasterModeFileLoaded;

    extern int MasterModeSlot;
};