#include "win32_main.h"

#include <stdio.h>
#include <string.h>

#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_vulkan.h>

#include "jarp/file.h"
#include "jarp/log.h"
#include "jarp/platform.h"
#include "jarp/shared.h"
#include "jarp/input/buttons.h"
#include "jarp/input/input.h"
#include "jarp/input/keys.h"
#include "jarp/renderer/vulkan/vk_renderer.h"

/*
====================
win32_test
====================
*/
PLATFORM_TEST(win32_test) {
    return 1;
}

/*
====================
win32_get_last_write_time
====================
*/
FILETIME win32_get_last_write_time(const char* filename) {
    FILETIME last_write_time = { 0 };
    WIN32_FILE_ATTRIBUTE_DATA data;
    BOOL success = GetFileAttributesExA(filename, GetFileExInfoStandard, &data);
    if (success) {
        last_write_time = data.ftLastWriteTime;
    }
    return last_write_time;
}

/*
====================
win32_unload_code
====================
*/
void win32_unload_code(struct Win32LoadedCode* loaded_code) {
    log_trace("Trying to unload game code");
    if (loaded_code->dll) {
        log_trace("Unloading game code");

        if (!FreeLibrary(loaded_code->dll)) {
            log_error("Failed unloading the DLL. Error code %d", GetLastError());
        }
        loaded_code->dll = 0;
    }
    loaded_code->is_valid = false;
    ZERO_ARRAY(loaded_code->function_count, loaded_code->functions);
}

/*
====================
win32_load_code
====================
*/
void win32_load_code(struct Win32LoadedCode* loaded_code) {
    WIN32_FILE_ATTRIBUTE_DATA ignored;
    if (!GetFileAttributesExA(loaded_code->full_lock_path, GetFileExInfoStandard, &ignored)) {
        log_trace("Loading game code");

        loaded_code->last_dll_write_time = win32_get_last_write_time(loaded_code->full_dll_path);
        if (!CopyFileA(loaded_code->full_dll_path, loaded_code->full_transient_dll_path, FALSE)) {
            log_error("Copying the DLL did not work. Error code %d", GetLastError());
        }

        loaded_code->dll = LoadLibraryA(loaded_code->full_transient_dll_path);
        if (loaded_code->dll) {
            loaded_code->is_valid = true;
            for (uint32_t i = 0; i < loaded_code->function_count; i++) {
                char* name = loaded_code->function_names[i];
                void* function = GetProcAddress(loaded_code->dll, loaded_code->function_names[i]);
                if (function) {
                    loaded_code->functions[i] = function;
                } else {
                    log_error("Failed loading DLL function. Error code %d", GetLastError());
                    loaded_code->is_valid = false;
                }
            }
        } else {
            log_error("Failed loading the DLL. Error code %d", GetLastError());
        }

        if (!loaded_code->is_valid) {
            log_error("Loaded code is invalid. Unloading the code again.");
            win32_unload_code(loaded_code);
        }
    }
}

/*
====================
win32_unload_game_code
====================
*/
void win32_unload_game_code(struct Win32GameCode* loaded_code) {
    if (loaded_code->dll) {
        if (!FreeLibrary(loaded_code->dll)) {
            log_error("Failed unloading the DLL. Error code %d", GetLastError());
        }
        loaded_code->dll = 0;
    }
    loaded_code->is_valid = false;
}

/*
====================
win32_get_game_api
====================
*/
struct GameExport* win32_get_game_api(struct Win32GameCode* loaded_code, struct GameImport* params) {
    GameGetAPI* game_get_api = NULL;

    if (loaded_code->dll) {
        log_trace("DLL not unloaded. Skipping loading the DLL again");
        return NULL;
    }

    WIN32_FILE_ATTRIBUTE_DATA ignored;
    if (!GetFileAttributesExW(loaded_code->full_lock_path, GetFileExInfoStandard, &ignored)) {
        loaded_code->last_dll_write_time = win32_get_last_write_time(loaded_code->full_dll_path);
        if (!CopyFileA(loaded_code->full_dll_path, loaded_code->full_transient_dll_path, FALSE)) {
            log_error("Copying the DLL did not work. Error code %d", GetLastError());
        }

        loaded_code->dll = LoadLibraryA(loaded_code->full_transient_dll_path);
        if (loaded_code->dll) {
            loaded_code->is_valid = true;
            void* function = GetProcAddress(loaded_code->dll, "game_get_api");
            if (function) {
                game_get_api = (GameGetAPI*)function;
            } else {
                log_fatal("Failed loading DLL function. Error code %d", GetLastError());
                loaded_code->is_valid = false;
            }
        }
        else {
            log_fatal("Failed loading the DLL. Error code %d", GetLastError());
        }

        if (!loaded_code->is_valid) {
            log_error("Loaded code is invalid. Unloading the code again.");
            win32_unload_game_code(loaded_code);
        }
    }

    if (game_get_api) {
        return game_get_api(params);
    } else {
        return NULL;
    }
}

/*
====================
main
====================
*/
int main(int argc, char** argv) {
    char* application_path = SDL_GetBasePath();
    //GetModuleFileNameA(0, application_path, sizeof(application_path));

    //struct Win32GameFunctionTable game = { 0 };
    //struct Win32LoadedCode game_code = { 0 };
    //strcpy(game_code.full_dll_path, application_path);
    //strcat(game_code.full_dll_path, "..\\Game\\Game.dll");
    //strcpy(game_code.full_transient_dll_path, application_path);
    //strcat(game_code.full_transient_dll_path, "..\\Game\\GameTemp.dll");
    //strcpy(game_code.full_lock_path, application_path);
    //strcat(game_code.full_lock_path, "..\\Game\\BuildLock.tmp");
    //game_code.function_count = ARRAY_COUNT(Win32GameFunctionTableNames);
    //game_code.function_names = Win32GameFunctionTableNames;
    //game_code.functions = (void**)&game;
    //win32_load_code(&game_code);

    struct Win32GameCode game_code = { 0 };
    strcpy(game_code.full_dll_path, application_path);
    strcat(game_code.full_dll_path, "..\\Game\\Game.dll");
    strcpy(game_code.full_transient_dll_path, application_path);
    strcat(game_code.full_transient_dll_path, "..\\Game\\GameTemp.dll");
    strcpy(game_code.full_lock_path, application_path);
    strcat(game_code.full_lock_path, "..\\Game\\BuildLock.tmp");
    struct GameExport ge = { 0 };
    struct GameImport gi = { 0 };
    gi.test = win32_test;
    ge = *win32_get_game_api(&game_code, &gi);

    struct Win32Window window;
    window.width = 800;
    window.height = 600;
    window.title = "jarp";

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    window.handle = SDL_CreateWindow(window.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     window.width, window.height, SDL_WINDOW_VULKAN);
    SDL_SetWindowResizable(window.handle, true);
    window.surface = SDL_GetWindowSurface(window.handle);
    
    SDL_SysWMinfo system_info;
    SDL_VERSION(&system_info.version);
    SDL_GetWindowWMInfo(window.handle, &system_info);
    window.hinstance = system_info.info.win.hinstance;
    window.hwnd = system_info.info.win.window;

    if (!vk_renderer_init(&window, application_path)) {
        vk_renderer_shutdown();
        return 1;
    }

    uint32_t current_fps_time = SDL_GetTicks();
    uint32_t last_fps_time = current_fps_time;
    uint32_t frames = 0;

    bool is_running = true;
    while (is_running) {
        // handle events
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_QUIT: {
                    is_running = false;
                    break;
                }
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                case SDL_TEXTEDITING:
                case SDL_TEXTINPUT:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION:
                case SDL_MOUSEWHEEL: {
                    input_event(&event);
                    break;
                }
                case SDL_WINDOWEVENT: {
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                        case SDL_WINDOWEVENT_RESIZED: {
                            window.width = event.window.data1;
                            window.height = event.window.data2;
                            log_trace("Window resized to (%d, %d)", event.window.data1, event.window.data2);
                            break;
                        }
                        default: {
                            log_warn("Unhandled window event type %d", event.window.event);
                            break;
                        }
                    }
                    break;
                }
                default: {
                    log_warn("Unhandled event type %d", event.type);
                    break;
                }
            }
        }

        // render and update stuff
        input_update();
        vk_renderer_update();
        vk_renderer_draw();

#if _DEBUG
        // reload game code if a change is detected
        bool game_code_reload_needed = false;
        FILETIME new_game_dll_write_time = win32_get_last_write_time(game_code.full_dll_path);
        game_code_reload_needed = CompareFileTime(&new_game_dll_write_time,
                                                  &game_code.last_dll_write_time) != 0;
        if (game_code_reload_needed) {
            ZERO_STRUCT(ge);
            win32_unload_game_code(&game_code);
            Sleep(100);
            ge = *win32_get_game_api(&game_code, &gi);
            Sleep(100);
        }
#endif

        // frame time
        current_fps_time = SDL_GetTicks();
        ++frames;
        if (current_fps_time > last_fps_time + 1000) {
            char buffer[32];
            sprintf(buffer, "%s - %d fps", window.title, frames);
            SDL_SetWindowTitle(window.handle, buffer);

            ge.update_and_render();

            last_fps_time = current_fps_time;
            frames = 0;
        }
    }

    SDL_DestroyWindow(window.handle);
    SDL_Quit();

    vk_renderer_shutdown();

    return 0;
}
