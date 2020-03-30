struct Win32LoadedCode
{
    HMODULE dll;
    FILETIME last_dll_write_time;

    const char* full_dll_path;
    const char* full_transient_dll_path;
    const char* full_lock_path;

    char* function_name;
    GameUpdateAndRender* update_and_render;
};
