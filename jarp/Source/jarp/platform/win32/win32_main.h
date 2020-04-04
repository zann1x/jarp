#include "jarp/api_types.h"

#include <Windows.h>

#include "jarp/platform.h"

struct Win32Window {
    uint32_t width;
    uint32_t height;
    const char* title;

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
