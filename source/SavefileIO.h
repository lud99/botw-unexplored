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

    extern std::vector<Data::Location*> visitedLocations;
    extern std::vector<Data::Location*> unexploredLocations;

    extern std::vector<Data::Hinox*> defeatedHinoxes;
    extern std::vector<Data::Hinox*> undefeatedHinoxes;

    extern std::vector<Data::Talus*> defeatedTaluses;
    extern std::vector<Data::Talus*> undefeatedTaluses;

    extern std::vector<Data::Molduga*> defeatedMoldugas;
    extern std::vector<Data::Molduga*> undefeatedMoldugas;

    uint32_t ReadU32(unsigned char* buffer, int offset);

    bool MountSavefile();
    bool UnmountSavefile();

    void CopySavefiles(u64 uid1, u64 uid2, bool* doneFlag);
    bool CopyFile(const std::string& file, const std::string& destination);

    bool ParseFile(const char* filepath);

    bool DirectoryExists(const std::string& filepath);

    extern u64 AccountUid1;
    extern u64 AccountUid2;
};