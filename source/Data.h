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

    extern Korok Koroks[900];

    Korok* KorokExists(uint32_t hash);

    // Create locations (coordinates are rounded)
    extern Location Locations[187];

    Location* LocationExists(uint32_t hash);

    extern Hinox Hinoxes[40];

    Hinox* HinoxExists(uint32_t hash);

    extern Talus Taluses[40];

    Talus* TalusExists(uint32_t hash);

    extern Molduga Moldugas[4];

    Molduga* MoldugaExists(uint32_t hash);
};