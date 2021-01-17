#pragma once

#include <vector>
#include <cstdint>

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

    bool ParseFile(const char* filepath);
};