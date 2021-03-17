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
#include "LoadGamesave.h"

bool openGLInitialized = false;
bool nxLinkInitialized = false;

void cleanUp()
{
    printf("Exiting, cleaning up...");

    // Cleanup
    if (Map::m_IsInitialized) Map::Destory();

    romfsExit();

    // Deinitialize EGL
    if (openGLInitialized) deinitEgl();

    // Deinitialize network 
    if (nxLinkInitialized) socketExit();
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