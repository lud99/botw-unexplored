#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include <EGL/egl.h>    // EGL library
#include <EGL/eglext.h> // EGL extensions
#include <glad/glad.h>  // glad library (OpenGL loader)
#include <stb/stb_image.h>

#include "InitGL.h"

#include "Level.h"

//-----------------------------------------------------------------------------
// Main program
//-----------------------------------------------------------------------------

static void setMesaConfig()
{
    // Uncomment below to disable error checking and save CPU time (useful for production):
    //setenv("MESA_NO_ERROR", "1", 1);

    // Uncomment below to enable Mesa logging:
    //setenv("EGL_LOG_LEVEL", "debug", 1);
    //setenv("MESA_VERBOSE", "all", 1);
    //setenv("NOUVEAU_MESA_DEBUG", "1", 1);

    // Uncomment below to enable shader debugging in Nouveau:
    //setenv("NV50_PROG_OPTIMIZE", "0", 1);
    //setenv("NV50_PROG_DEBUG", "1", 1);
    //setenv("NV50_PROG_CHIPSET", "0x120", 1);
}

int main(int argc, char* argv[])
{
    // Setup NXLink
    socketInitializeDefault();
    nxlinkStdio();

    // Set mesa configuration (useful for debugging)
    setMesaConfig();

    // Init romfs
    romfsInit();

    // Initialize EGL on the default window
    if (!initEgl(nwindowGetDefault()))
        return EXIT_FAILURE;

    // Load OpenGL routines using glad
    gladLoadGL();

    // Initialize our scene
    Level::Init(nwindowGetDefault());

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    hidInitializeTouchScreen();

    // Main graphics loop
    while (appletMainLoop())
    {
        glm::vec3 color = glm::vec3(78, 192, 202) / 255.0f;

        glClearColor(color.x, color.y, color.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Get and process input
        padUpdate(&pad);
        u32 kDown = padGetButtonsDown(&pad);
        if (kDown & HidNpadButton_Plus)
            break;

        Level::ProcessInput(pad);

        Level::Update();

        Level::Draw();

        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            printf("OpenGL error: %u\n", err);
        }

        eglSwapBuffers(s_display, s_surface);
    }

    // Deinitialize our scene
    Level::Exit();

    // Deinitialize EGL
    deinitEgl();

    // Deinitialize newtork 
    socketExit();

    return EXIT_SUCCESS;
}