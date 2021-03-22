#include <fstream>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include <chrono>
#include <dirent.h>

#include <switch.h>

#include "SavefileIO.h"
#include "Graphics/InitGL.h"
#include "Graphics/Font.h"
#include "Map.h"
#include "Accounts.h"
#include "Dialog.h"
#include "LoadGamesave.h"
#include "Graphics/LineRenderer.h"

bool openGLInitialized = false;
bool nxLinkInitialized = false;
int s_nxlinkSock = -1;

static void deinitNxLink()
{
    if (s_nxlinkSock >= 0)
    {
        close(s_nxlinkSock);
        socketExit();
        s_nxlinkSock = -1;
    }
}

void cleanUp()
{
    // Save settings
    if (!SavefileIO::DirectoryExists("sdmc:/switch/botw-unexplored"))
        mkdir("sdmc:/switch/botw-unexplored", 0777);

    std::ofstream file("sdmc:/switch/botw-unexplored/settings.txt");
    if (file.is_open())
    {
        file << Map::m_CameraPosition.x << "\n";
        file << Map::m_CameraPosition.y << "\n";
        file << Map::m_Zoom << "\n";
        printf("Saved settings\n");
    } else
        printf("Failed top open settings file (cleanUp())\n");
    file.close();

    Map::Destory();

    // Cleanup
    romfsExit();

    // Deinitialize EGL
    deinitEgl();

    // Deinitialize network 
    deinitNxLink();
    socketExit();
}

int main()
{
    appletLockExit(); // To be able to run memory cleanup when the app closes

    // Setup NXLink
    socketInitializeDefault();
    s_nxlinkSock = nxlinkStdio();
    nxLinkInitialized = true;

    // Init romfs
    romfsInit();
    
    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // OpenGL
    // Initialize EGL on the default window
    openGLInitialized = initEgl(nwindowGetDefault());
    if (!openGLInitialized)
        return 0;

    // Load OpenGL routines using glad
    gladLoadGL();

    // OpenGL config
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);
    
    Map::Init();
    Map::m_Pad = &pad;

    // Load settings if they exist
    std::ifstream settingsFile("sdmc:/switch/botw-unexplored/settings.txt");
    if (settingsFile.is_open())
    {
        std::string line;

        std::getline(settingsFile, line);
        Map::m_CameraPosition.x = std::stof(line);

        std::getline(settingsFile, line);
        Map::m_CameraPosition.y = std::stof(line);

        std::getline(settingsFile, line);
        Map::m_Zoom = std::stof(line);

        if (Map::m_CameraPosition.x > 4500.0f)
            Map::m_CameraPosition.x = 4250.0f;
        if (Map::m_CameraPosition.x < -4500.0f)
            Map::m_CameraPosition.x = -4250.0f;
        if (Map::m_CameraPosition.y > 2500.0f)
            Map::m_CameraPosition.y = 2250.0f;
        if (Map::m_CameraPosition.y < -2000.0f)
            Map::m_CameraPosition.y = -1750.0f;
    } else {
        printf("Failed to open settings file\n");
    }

    settingsFile.close();
    
    bool hasDoneFirstDraw = false;
    bool hasLoadedSave = false;

    LineRenderer* line = new LineRenderer();

    glm::vec2 s(0.0f);
    glm::vec2 e(0.0f);

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

        // // Render
        Map::Render();

        if (hasDoneFirstDraw && !hasLoadedSave)
        {
            hasLoadedSave = true;
            LoadGamesave();

            Map::UpdateMapObjects();
        }

        HidAnalogStickState analog_stick_l = padGetStickPos(&pad, 0);

        // Get the stick position between -1.0f and 1.0f, instead of -32767 and 32767
        glm::vec2 stickLPosition = glm::vec2(
            ((float)analog_stick_l.x / (float)JOYSTICK_MAX), 
            ((float)analog_stick_l.y / (float)JOYSTICK_MAX));

        //std::cout << stickLPosition.x << ", " << stickLPosition.y << "\n";
    
        float deadzone = 0.1f;
        float distanceToCenter = glm::distance(stickLPosition, glm::vec2(0.0f, 0.0f));
        if (distanceToCenter >= deadzone)
            s = stickLPosition * 100.0f;
        
        HidAnalogStickState analog_stick_r = padGetStickPos(&pad, 1);

        // Get the stick position between -1.0f and 1.0f, instead of -32767 and 32767
        glm::vec2 stickRPosition = glm::vec2(
            ((float)analog_stick_r.x / (float)JOYSTICK_MAX), 
            ((float)analog_stick_r.y / (float)JOYSTICK_MAX));
    
        deadzone = 0.1f;
        distanceToCenter = glm::distance(stickRPosition, glm::vec2(0.0f, 0.0f));
        if (distanceToCenter >= deadzone)
            e = stickRPosition * 100.0f;

        line->AddLine(s, e, 10.0f);
        line->RenderLines(Map::m_ProjectionMatrix);

        eglSwapBuffers(s_display, s_surface);

        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            printf("OpenGL error: %u\n", err);
        }

        hasDoneFirstDraw = true;
	}

    cleanUp();

    appletUnlockExit(); // Exit the app

    return EXIT_SUCCESS;
}