#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#include "SavefileIO.h"
#include "Map.h"
#include "Dialog.h"

static bool LoadGamesave(bool loadMasterMode = false, bool chooseProfile = false)
{   
    printf("Loading gamesave...\n");
    // Try to mount the save directory
    int mountStatus = SavefileIO::MountSavefile(!chooseProfile);

    bool dialogWasOpen = Map::m_GameRunningDialog->m_IsOpen || Map::m_MasterModeDialog->m_IsOpen || Map::m_NoSavefileDialog->m_IsOpen;

    Map::m_GameRunningDialog->SetOpen(false);
    Map::m_MasterModeDialog->SetOpen(false);
    Map::m_NoSavefileDialog->SetOpen(false);

    if (mountStatus == 1) { // Good
        Map::m_GameRunningDialog->SetOpen(false);

        if (loadMasterMode)
        {
            bool success = false;
            if (SavefileIO::MasterModeFileExists) 
            {
                std::string path = "save:/" + std::to_string(SavefileIO::MostRecentMasterModeFile) + "/game_data.sav";
                success = SavefileIO::ParseFile(path.c_str());
            }

            if (success) SavefileIO::MasterModeFileLoaded = true;

            SavefileIO::UnmountSavefile();
                
            return success;
        } else
            SavefileIO::MasterModeFileLoaded = false;

        std::string path = "save:/" + std::to_string(SavefileIO::MostRecentNormalModeFile) + "/game_data.sav";
        bool success = SavefileIO::ParseFile(path.c_str());
        if (!success) 
        {
            SavefileIO::UnmountSavefile();
            Map::m_NoSavefileDialog->SetOpen(true);
            return false;
        }

        if (dialogWasOpen)
            Map::m_IsLegendOpen = true;

        SavefileIO::CopySavefiles();

        SavefileIO::UnmountSavefile();
    } 
    // Failed to mount it. Can happen if no profile was choosen, or some other account thing went wrong 
    else if (mountStatus == 0) {  // No save for profile
        printf("Canceled profile picker\n");

        return false;
    } else if (mountStatus == -1) { // Game is running
        printf("Game is running. Loading backup...\n");

        bool loadedBackupSuccess = SavefileIO::LoadBackup(loadMasterMode);

        if (!loadedBackupSuccess) 
        {
            SavefileIO::LoadedSavefile = false;
            SavefileIO::MasterModeFileLoaded = false;
            Map::m_GameRunningDialog->SetOpen(true);

            return false;
        }

        if (dialogWasOpen)
            Map::m_IsLegendOpen = true;
    } else if (mountStatus == -2) { // User has no save data
        printf("The selected user has no save data\n");

        SavefileIO::MasterModeFileLoaded = false;
        Map::m_NoSavefileDialog->SetOpen(true);
        
        SavefileIO::UnmountSavefile();

        return false;
    }

    return true;
}