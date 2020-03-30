struct Win32LoadedCode
{
    HMODULE dll;
    FILETIME last_dll_write_time;

    char full_dll_path[MAX_PATH];
    char full_transient_dll_path[MAX_PATH];
    char full_lock_path[MAX_PATH];

    char* function_name;
    GameUpdateAndRender* update_and_render;
};
