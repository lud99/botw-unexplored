#include <fstream>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <vector>

#include <switch.h>

#include "SavefileIO.h"
#include "Graphics/InitGL.h"
#include "Map.h"

/*printf("%u korok seeds have been found, %u are missing\n", foundKoroks.size(), missingKoroks.size());
printf("%u locations have been visited, %u are missing\n", visitedLocations.size(), unexploredLocations.size());
printf("%u hinoxes have been defeated, %u remains\n", defeatedHinoxes.size(), undefeatedHinoxes.size());
printf("%u taluses have been defeated, %u remains\n", defeatedTaluses.size(), undefeatedTaluses.size());
printf("%u moldugas have been defeated, %u remains\n", defeatedMoldugas.size(), undefeatedMoldugas.size());*/


Map* map;

int main()
{
    // Setup NXLink
    socketInitializeDefault();
    nxlinkStdio();

    // Init romfs
    romfsInit();

    // Initialize EGL on the default window
    if (!initEgl(nwindowGetDefault()))
        return EXIT_FAILURE;

    // Load OpenGL routines using glad
    gladLoadGL();

    // OpenGL config
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    // Load gamesave

	SavefileIO::ParseFile("romfs:/game_data.sav");

    map = new Map();
    map->m_Pad = &pad;

	while (appletMainLoop())
	{
        // Scan the gamepad. This should be done once for each frame
        padUpdate(&pad);

        if (padGetButtons(&pad) & HidNpadButton_Plus)
            break;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Update
        map->Update();

        // Render
        map->Render();

        eglSwapBuffers(s_display, s_surface);

        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            printf("OpenGL error: %u\n", err);
        }
	}

    // Cleanup
    delete map;

    romfsExit();

    // Deinitialize EGL
    deinitEgl();

    // Deinitialize newtork 
    socketExit();

    return EXIT_SUCCESS;
}