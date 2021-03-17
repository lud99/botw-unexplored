#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#include "SavefileIO.h"
#include "Map.h"
#include "Dialog.h"
#include "Graphics/InitGL.h"

static bool LoadGamesave(bool loadMasterMode = false)
{   
    // Try to mount the save directory
    int mountStatus = SavefileIO::MountSavefile(loadMasterMode);

    Map::m_GameRunningDialog->SetOpen(false);
    Map::m_MasterModeDialog->SetOpen(false);

    glm::mat4 empty(1.0f);
    Map::m_Font.m_ViewMatrix = &empty; 

    if (mountStatus == 1) { // Good
        // Draw loading text
        Map::m_Font.RenderText("Loading savefile...", glm::vec2(0.0f, 0.0f), 1.0f, glm::vec3(1.0f), ALIGN_CENTER);
        eglSwapBuffers(s_display, s_surface);

        Map::m_GameRunningDialog->SetOpen(false);

        if (loadMasterMode)
        {
            bool success = false;
            if (SavefileIO::MasterModeFileExists) 
            {
                std::string path = "save:/" + std::to_string(SavefileIO::MasterModeSlot) + "/game_data.sav";
                success = SavefileIO::ParseFile(path.c_str());
            }

            if (success) 
            {
                //Map::m_IsLegendOpen = true;
                SavefileIO::MasterModeFileLoaded = true;
            }

            SavefileIO::UnmountSavefile();
                
            return success;
        } else
            SavefileIO::MasterModeFileLoaded = false;

        bool success = SavefileIO::ParseFile("save:/0/game_data.sav");
        if (!success) 
        {
            SavefileIO::UnmountSavefile();
            Map::m_NoSavefileDialog->SetOpen(true);
            return false;
        }

        SavefileIO::CopySavefiles(SavefileIO::AccountUid1, SavefileIO::AccountUid2);

        SavefileIO::UnmountSavefile();

        Map::m_NoSavefileDialog->SetOpen(false);

        if (SavefileIO::MasterModeFileExists) 
        {
            std::cout << "Master mode savefile " << SavefileIO::MasterModeSlot << " exists\n";
            //Map::m_MasterModeDialog->SetOpen(true);
        } else {
            
        }

        //Map::m_IsLegendOpen = true;
    } 
    // Failed to mount it. Can happen if no profile was choosen, or some other account thing went wrong 
    else if (mountStatus == 0) {  // No save for profile
        printf("Canceled profile picker\n");

        return false;
    } else if (mountStatus == -1) { // Game is running
        printf("Game is running. Loading backup...\n");

        // Draw loading text
        Map::m_Font.RenderText("Loading savefile...", glm::vec2(0.0f, 0.0f), 1.0f, glm::vec3(1.0f), ALIGN_CENTER);
        eglSwapBuffers(s_display, s_surface);

        if (loadMasterMode)
        {
            bool success = SavefileIO::LoadBackup(std::to_string(SavefileIO::MasterModeSlot));

            if (success) 
            {
                //Map::m_IsLegendOpen = true;
                SavefileIO::MasterModeFileLoaded = true;
            }

            SavefileIO::UnmountSavefile();

            return success;
        } else
            SavefileIO::MasterModeFileLoaded = false;
        
        bool loadedBackupSuccess = SavefileIO::LoadBackup("0");

        if (!loadedBackupSuccess) 
        {
            SavefileIO::LoadedSavefile = false;
            Map::m_NoSavefileDialog->SetOpen(false);
            Map::m_GameRunningDialog->SetOpen(true);

            return false;
        }

        Map::m_NoSavefileDialog->SetOpen(false);

        if (SavefileIO::MasterModeFileExists) 
        {
            std::cout << "Master mode savefile " << SavefileIO::MasterModeSlot << " exists\n";
            //Map::m_MasterModeDialog->SetOpen(true);
        } else {
            
        }

        //Map::m_IsLegendOpen = true;
    } else if (mountStatus == -2) { // User has no save data
        printf("The selected user has no save data\n");

        SavefileIO::MasterModeFileLoaded = false;
        Map::m_NoSavefileDialog->SetOpen(true);
        return false;
    }

    return true;
}