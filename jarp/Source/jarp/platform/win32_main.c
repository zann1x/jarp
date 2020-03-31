#include <stdio.h>
#include <string.h>
#include <Windows.h>

#include <SDL.h>

#include "jarp/file.h"
#include "jarp/log.h"
#include "jarp/platform.h"
#include "jarp/shared.h"
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

void win32_unload_code(struct Win32LoadedCode* loaded_code)
{
    if (loaded_code->dll)
    {
        log_trace("Unloading game code");

        if (!FreeLibrary(loaded_code->dll))
            log_error("Failed unloading the DLL. Error code %d", GetLastError());

        loaded_code->dll = 0;
    }
    loaded_code->is_valid = false;
    ZERO_ARRAY(loaded_code->function_count, loaded_code->functions);
}

void win32_load_code(struct Win32LoadedCode* loaded_code)
{
    WIN32_FILE_ATTRIBUTE_DATA ignored;
    if (!GetFileAttributesExA(loaded_code->full_lock_path, GetFileExInfoStandard, &ignored))
    {
        log_trace("Loading game code");

        loaded_code->last_dll_write_time = win32_get_last_write_time(loaded_code->full_dll_path);

        if (!CopyFileA(loaded_code->full_dll_path, loaded_code->full_transient_dll_path, FALSE))
            log_error("Copying the DLL did not work. Error code %d", GetLastError());

        loaded_code->dll = LoadLibraryA(loaded_code->full_transient_dll_path);
        if (loaded_code->dll)
        {
            loaded_code->is_valid = true;
            for (uint32_t i = 0; i < loaded_code->function_count; i++)
            {
                char* name = loaded_code->function_names[i];
                void* function = GetProcAddress(loaded_code->dll, loaded_code->function_names[i]);
                if (function)
                {
                    loaded_code->functions[i] = function;
                }
                else
                {
                    log_error("Failed loading DLL function. Error code %d", GetLastError());
                    loaded_code->is_valid = false;
                }
            }
        }
        else
        {
            log_error("Failed loading the DLL. Error code %d", GetLastError());
        }

        if (!loaded_code->is_valid)
        {
            log_error("Loaded code is invalid. Unloading the code again.");
            win32_unload_code(&loaded_code);
        }
    }
}

PLATFORM_TEST(win32_test)
{
    return 1;
}

int main(int argc, char** argv)
{
    char* application_path = SDL_GetBasePath();
    //GetModuleFileNameA(0, application_path, sizeof(application_path));

    struct Win32GameFunctionTable game = { 0 };
    struct Win32LoadedCode game_code = { 0 };
    strcpy(game_code.full_dll_path, application_path);
    strcat(game_code.full_dll_path, "..\\Game\\Game.dll");
    strcpy(game_code.full_transient_dll_path, application_path);
    strcat(game_code.full_transient_dll_path, "..\\Game\\GameTemp.dll");
    strcpy(game_code.full_lock_path, application_path);
    strcat(game_code.full_lock_path, "..\\Game\\BuildLock.tmp");
    game_code.function_count = ARRAY_COUNT(Win32GameFunctionTableNames);
    game_code.function_names = Win32GameFunctionTableNames;
    game_code.functions = (void**)&game;
    win32_load_code(&game_code);

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
            win32_unload_code(&game_code);
            win32_load_code(&game_code);
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

            game.update_and_render(&game_memory);

            last_fps_time = current_fps_time;
            frames = 0;
        }
    }

    window_destroy();

    return 0;
}
