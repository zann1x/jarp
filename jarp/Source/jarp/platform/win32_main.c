#include <stdio.h>
#include <Windows.h>

#include <SDL.h>

#include "jarp/api_types.h"
#include "jarp/file.h"
#include "jarp/log.h"
#include "jarp/platform.h"
#include "jarp/window.h"
#include "jarp/input/buttons.h"
#include "jarp/input/input.h"
#include "jarp/input/keys.h"
#include "jarp/platform/win32_main.h"

bool is_running;

void handle_events(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
            case SDL_QUIT:
            {
                is_running = false;
                break;
            }
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEMOTION:
            case SDL_MOUSEWHEEL:
            {
                input_event(&event);
                break;
            }
            case SDL_WINDOWEVENT:
            {
                window_event(&event);
                break;
            }
        }
    }
}

FILETIME win32_get_last_write_time(const char* filename)
{
    FILETIME last_write_time = { 0 };
    WIN32_FILE_ATTRIBUTE_DATA data;

    BOOL success = GetFileAttributesExA(filename, GetFileExInfoStandard, &data);
    if (success)
    {
        last_write_time = data.ftLastWriteTime;
    }

    return last_write_time;
}

void load_code(struct Win32LoadedCode* loaded_code)
{
    WIN32_FILE_ATTRIBUTE_DATA ignored;
    if (!GetFileAttributesExA(loaded_code->full_lock_path, GetFileExInfoStandard, &ignored))
    {
        log_trace("Loading game code");

        loaded_code->last_dll_write_time = win32_get_last_write_time(loaded_code->full_dll_path);

        if (!CopyFileA(loaded_code->full_dll_path, loaded_code->full_transient_dll_path, FALSE))
            log_error("Copying the DLL did not work. Error code %d", GetLastError());

        loaded_code->dll = LoadLibraryA(loaded_code->full_transient_dll_path);
        if (!loaded_code->dll)
        {
            log_error("Failed loading the DLL. Error code %d", GetLastError());
        }
        else
        {
            loaded_code->update_and_render = (GameUpdateAndRender*)GetProcAddress(loaded_code->dll,
                                                                                  loaded_code->function_name);

            if (!loaded_code->update_and_render)
                log_error("Failed loading DLL function. Error code %d", GetLastError());
        }
    }
}

void unload_code(struct Win32LoadedCode* loaded_code)
{
    if (loaded_code->dll)
    {
        log_trace("Unloading game code");

        if (!FreeLibrary(loaded_code->dll))
            log_error("Failed unloading the DLL. Error code %d", GetLastError());

        loaded_code->dll = 0;
        loaded_code->update_and_render = NULL;
    }
}

PLATFORM_TEST(win32_test)
{
    return 1;
}

int main(int argc, char** argv)
{
    struct Win32LoadedCode game_code;
    game_code.full_dll_path = "E:/code/jarp/Game/Binaries/Debug-windows-x86_64/Game.dll";
    game_code.full_transient_dll_path = "E:/code/jarp/Game/Binaries/Debug-windows-x86_64/Game_temp.dll";
    game_code.full_lock_path = "E:/code/jarp/Game/Binaries/Debug-windows-x86_64/BuildLock.tmp";
    game_code.function_name = "game_update_and_render";
    load_code(&game_code);

    struct PlatformAPI platform_api;
    platform_api.test = win32_test;

    struct GameMemory game_memory;
    game_memory.platform_api = platform_api;

    window_init();

    uint32_t current_fps_time = SDL_GetTicks();
    uint32_t last_fps_time = current_fps_time;
    uint32_t frames = 0;

    is_running = true;
    while (is_running)
    {
        handle_events();

        // render and update stuff
        input_update();

#if _DEBUG
        // reload game code if a change is detected
        bool game_code_reload_needed = false;
        FILETIME new_game_dll_write_time = win32_get_last_write_time(game_code.full_dll_path);
        game_code_reload_needed = CompareFileTime(&new_game_dll_write_time,
                                                  &game_code.last_dll_write_time) != 0;
        if (game_code_reload_needed)
        {
            unload_code(&game_code);
            load_code(&game_code);
        }
#endif

        // frame time
        current_fps_time = SDL_GetTicks();
        ++frames;
        if (current_fps_time > last_fps_time + 1000)
        {
            char buffer[32];
            sprintf(buffer, "%s - %d fps", window.title, frames);
            window_set_display_title(buffer);

            game_code.update_and_render(&game_memory);

            last_fps_time = current_fps_time;
            frames = 0;
        }
    }

    window_destroy();

    return 0;
}
