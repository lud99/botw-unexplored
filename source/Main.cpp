#include <fstream>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <vector>
#include <thread>
#include <chrono>

#include <switch.h>

#include "SavefileIO.h"
#include "Graphics/InitGL.h"
#include "Graphics/Font.h"
#include "Map.h"
#include "Accounts.h"
#include "Dialog.h"

bool openGLInitialized = false;
bool nxLinkInitialized = false;

void cleanUp();

bool LoadGamesave(bool loadMasterMode = false)
{   
    // Try to mount the save directory
    int mountStatus = SavefileIO::MountSavefile(!loadMasterMode);

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

            if (success) Map::m_IsLegendOpen = true;

            SavefileIO::UnmountSavefile();
                
            return success;
        }

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
            Map::m_MasterModeDialog->SetOpen(true);
        } else {
            Map::m_IsLegendOpen = true;
        }
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

            if (success) Map::m_IsLegendOpen = true;

            SavefileIO::UnmountSavefile();

            return success;
        }
        
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
            Map::m_MasterModeDialog->SetOpen(true);
        } else {
            Map::m_IsLegendOpen = true;
        }
    } else if (mountStatus == -2) { // User has no save data
        printf("The selected user has no save data\n");

        Map::m_NoSavefileDialog->SetOpen(true);
        return false;
    }

    return true;
}

int main()
{
    // Setup NXLink
    socketInitializeDefault();
    nxlinkStdio();
    nxLinkInitialized = true;

    // Init romfs
    romfsInit();

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);


    // OpenGL
    // Initialize EGL on the default window
    openGLInitialized = initEgl(nwindowGetDefault());
    if (!openGLInitialized)
        return EXIT_FAILURE;

    // Load OpenGL routines using glad
    gladLoadGL();

    // OpenGL config
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    
    Map::Init();
    Map::m_Pad = &pad;

    bool firstDraw = true;
    bool hasDoneDirstDraw = false;

	while (appletMainLoop())
	{
        // Scan the gamepad. This should be done once for each frame
        padUpdate(&pad);

        u64 buttonsDown = padGetButtonsDown(&pad);

        if (buttonsDown & HidNpadButton_Plus)
            break;

        if (Map::m_ShouldExit)
            break;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Update
        Map::Update();

        // Render
        Map::Render();

        if (Map::m_ShouldChooseProfile)
        {
            Map::m_ShouldChooseProfile = false;

            LoadGamesave();

            Map::m_Font.m_ViewMatrix = &Map::m_ViewMatrix;

            Map::UpdateMapObjects();
        }

        if (Map::m_ShouldLoadMastermodeFile)
        {
            Map::m_ShouldLoadMastermodeFile = false;

            LoadGamesave(true);

            Map::m_Font.m_ViewMatrix = &Map::m_ViewMatrix;

            Map::UpdateMapObjects();
        }

        if (firstDraw && hasDoneDirstDraw)
        {
            LoadGamesave();

            Map::m_Font.m_ViewMatrix = &Map::m_ViewMatrix;

            Map::UpdateMapObjects();
            firstDraw = false;
        }

        eglSwapBuffers(s_display, s_surface);

        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            printf("OpenGL error: %u\n", err);
        }

        hasDoneDirstDraw = true;
	}

    cleanUp();

    return EXIT_SUCCESS;
}

void cleanUp()
{
    printf("Exiting, cleaning up...");

    // printf("Waiting for thread...");
    // while (true)
    // {
    //     if (SavefileIO::FinishedCopying)
    //         break;
    // };
        
    // Cleanup
    if (Map::m_IsInitialized) Map::Destory();

    romfsExit();

    // Deinitialize EGL
    if (openGLInitialized) deinitEgl();

    // Deinitialize network 
    if (nxLinkInitialized) socketExit();
}