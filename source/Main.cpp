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

bool openGLInitialized = false;
bool nxLinkInitialized = false;

void cleanUp();

bool LoadGamesave()
{   
    // Try to mount the save directory
    int mountStatus = SavefileIO::MountSavefile();

    if (mountStatus == 1) { // Good
        bool success = SavefileIO::ParseFile("save:/0/game_data.sav");

        SavefileIO::CopySavefiles(SavefileIO::AccountUid1, SavefileIO::AccountUid2);
        SavefileIO::UnmountSavefile();

        if (!success) return false;

        return true;
    } 
    // Failed to mount it. Can happen if no profile was choosen, or some other account thing went wrong 
    else if (mountStatus == 0) {  // No save for profile
        printf("No save exists for that profile\n");

        return false;
    } else if (mountStatus == -1) { // Game is running
        printf("Game is running. Loading backup...\n");
        
        return SavefileIO::LoadBackup("0");
    }

    return false;
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

    // {
        // printf("Failed to open the savefile from that user. Make sure the user has a save file\n");

        // //cleanUp();

        // return EXIT_SUCCESS;
    // } else {
    //     printf("Loaded savefile\n");
    // }

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

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Update
        Map::Update();

        // Render
        Map::Render();

        if (firstDraw && hasDoneDirstDraw)
        {
            Map::m_Font.RenderText("Loading savefile...", glm::vec2(0.0f, 0.0f), 1.5f, glm::vec3(1.0f), ALIGN_CENTER);
            eglSwapBuffers(s_display, s_surface);

            LoadGamesave();
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