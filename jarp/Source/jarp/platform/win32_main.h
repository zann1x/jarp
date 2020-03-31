#include "jarp/api_types.h"

struct Win32LoadedCode
{
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

struct Win32GameFunctionTable
{
    GameUpdateAndRender* update_and_render;
};
static char* Win32GameFunctionTableNames[] = {
    "game_update_and_render"
};
