#pragma once

#include <cstdint>
#include <string>

namespace Data
{
    // For korok names and positions, https://github.com/d4mation/botw-unexplored-viewer/blob/master/assets/js/map-locations.js

    struct Korok
    {
        uint32_t hash;

        std::string internalName;
        std::string displayName = "Korok";

        float x = 0;
        float y = 0;

        Korok(uint32_t hash, const std::string& internalName, float x, float y) :
            hash(hash), internalName(internalName), x(x), y(y) {};
    };

    // For location names and their positions, https://github.com/MrCheeze/botw-waypoint-map/blob/gh-pages/map_locations.js
    // For the location hashes, https://github.com/marcrobledo/savegame-editors/blob/master/zelda-botw/zelda-botw.locations.js

    struct Location
    {
        uint32_t hash;

        std::string internalName;
        std::string displayName;

        float x = 0;
        float y = 0;

        Location(uint32_t hash, const std::string& internalName, const std::string& displayName, float x, float y) :
            hash(hash), internalName(internalName), displayName(displayName), x(x), y(y) {};
    };

    // Hash and positions from https://github.com/marcrobledo/savegame-editors/blob/master/zelda-botw/zelda-botw.data.js  BOTW_DATA.COORDS[BOTW_Data.DEFEATED_HINOX[i]]

    struct Hinox
    {
        uint32_t hash;

        float x = 0;
        float y = 0;

        Hinox(uint32_t hash, float x, float y) :
            hash(hash), x(x), y(y) {};
    };

    // Hash and positions from https://github.com/marcrobledo/savegame-editors/blob/master/zelda-botw/zelda-botw.data.js  BOTW_DATA.COORDS[BOTW_Data.DEFEATED_TALUS[i]]

    struct Talus
    {
        uint32_t hash;

        float x = 0;
        float y = 0;

        Talus(uint32_t hash, float x, float y) :
            hash(hash), x(x), y(y) {};
    };

    // Hash and positions from https://github.com/marcrobledo/savegame-editors/blob/master/zelda-botw/zelda-botw.data.js  BOTW_DATA.COORDS[BOTW_Data.DEFEATED_MOLDUGA[i]]

    struct Molduga
    {
        uint32_t hash;

        float x = 0;
        float y = 0;

        Molduga(uint32_t hash, float x, float y) :
            hash(hash), x(x), y(y) {};
    };

    // Create koroks

    const int KoroksCount = 900;
    extern Korok Koroks[KoroksCount];

    Korok* KorokExists(uint32_t hash);

    // Create locations (coordinates are rounded)
    const int LocationsCount = 187;
    extern Location Locations[LocationsCount];

    Location* LocationExists(uint32_t hash);

    const int HinoxesCount = 40;
    extern Hinox Hinoxes[HinoxesCount];

    Hinox* HinoxExists(uint32_t hash);

    const int TalusesCount = 40;
    extern Talus Taluses[TalusesCount];

    Talus* TalusExists(uint32_t hash);

    const int MoldugasCount = 4;
    extern Molduga Moldugas[MoldugasCount];

    Molduga* MoldugaExists(uint32_t hash);
};