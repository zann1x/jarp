#ifndef WIN32_MAIN_H
#define WIN32_MAIN_H

// Generally useful standard headers for more types compliant with C99
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

// Standard library stuff
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

// Our own included libraries
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_vulkan.h>
#include <stb_image.h>
#include <volk.h>

// Our own code
#include "jarp/api_types.h"
#include "jarp/shared.h"
#include "jarp/platform.h"
#include "jarp/input/input.h"
#include "jarp/log.h"
#include "jarp/file.h"
#include "jarp/math/math.h"
#include "jarp/mesh.h"
#include "jarp/module.h"
#include "jarp/renderer/camera.h"
#include "jarp/renderer/texture.h"
#include "jarp/renderer/vulkan/vk_renderer.h"

struct Win32Window {
    uint32_t width;
    uint32_t height;
    const char* title;

    bool is_framebuffer_resized;
    bool is_minimized;

    struct SDL_Window* handle;
    struct SDL_Surface* surface;

    HWND hwnd;
    HINSTANCE hinstance;
};

struct Win32LoadedCode {
    bool is_valid;

    HMODULE dll;
    FILETIME last_dll_write_time;

    char full_dll_path[MAX_PATH];
    char full_transient_dll_path[MAX_PATH];
    char full_lock_path[MAX_PATH];

    uint32_t function_count;
    char** function_names;
    void** functions;
};

struct Win32GameCode {
    bool is_valid;

    HMODULE dll;
    FILETIME last_dll_write_time;

    char full_dll_path[MAX_PATH];
    char full_transient_dll_path[MAX_PATH];
    char full_lock_path[MAX_PATH];
};

struct Win32GameFunctionTable {
    GameUpdateAndRender* game_update_and_render;
};
static char* Win32GameFunctionTableNames[] = {
    "game_update_and_render"
};

#endif
