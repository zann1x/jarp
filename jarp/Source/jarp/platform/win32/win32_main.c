#include "win32_main.h"

#include "../ThirdParty/stb/stb_image_impl.c"
#include "../ThirdParty/volk/volk.c"

#include "jarp/log.c"
#include "jarp/file.c"
#include "jarp/module.c"
#include "jarp/renderer/camera.c"
#include "jarp/renderer/texture.c"
#include "jarp/renderer/vulkan/vk_renderer.c"

static int win32_input_mouse_x = 0;
static int win32_input_mouse_y = 0;
static uint16_t win32_input_key_down[JARP_KEY_COUNT];
static uint8_t win32_input_button_down[JARP_BUTTON_COUNT];

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
    LPCWSTR lock_path = (LPCWSTR)loaded_code->full_lock_path;
    if (!GetFileAttributesExW(lock_path, GetFileExInfoStandard, &ignored)) {
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

enum EKey win32_input_remap_key(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_UNKNOWN:  return JARP_KEY_UNKNOWN;
        case SDL_SCANCODE_A:        return JARP_KEY_A;
        case SDL_SCANCODE_D:        return JARP_KEY_D;
        case SDL_SCANCODE_S:        return JARP_KEY_S;
        case SDL_SCANCODE_W:        return JARP_KEY_W;
        case SDL_SCANCODE_SPACE:    return JARP_KEY_SPACE;
        default:                    return JARP_KEY_UNKNOWN;
    }
}

enum EButton win32_input_remap_button(int32_t button) {
    switch (button) {
        case SDL_BUTTON_LEFT:   return JARP_BUTTON_LEFT;
        case SDL_BUTTON_MIDDLE: return JARP_BUTTON_MIDDLE;
        case SDL_BUTTON_RIGHT:  return JARP_BUTTON_RIGHT;
        case SDL_BUTTON_X1:     return JARP_BUTTON_BACK;
        case SDL_BUTTON_X2:     return JARP_BUTTON_FORWARD;
        default:                return JARP_BUTTON_UNKNOWN;
    }
}

void win32_input_update(void) {
    if (SDL_GetRelativeMouseMode() == true) {
        SDL_GetRelativeMouseState(&win32_input_mouse_x, &win32_input_mouse_y);
    } else {
        SDL_GetMouseState(&win32_input_mouse_x, &win32_input_mouse_y);
    }
}

/*
====================
main
====================
*/
int main(int argc, char** argv) {
    char* application_path = SDL_GetBasePath();

    struct Win32GameCode game_code = { 0 };
    strcpy(game_code.full_dll_path, application_path);
    strcat(game_code.full_dll_path, "Game.dll");
    strcpy(game_code.full_transient_dll_path, application_path);
    strcat(game_code.full_transient_dll_path, "GameTemp.dll");
    strcpy(game_code.full_lock_path, application_path);
    strcat(game_code.full_lock_path, "BuildLock.tmp");
    struct GameExport ge = { 0 };
    struct GameImport gi = { 0 };
    gi.test = win32_test;
    ge = *win32_get_game_api(&game_code, &gi);

    struct Win32Window window;
    window.width = 800;
    window.height = 600;
    window.title = "jarp";

    window.is_framebuffer_resized = false;
    window.is_minimized = false;

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

    struct Camera camera;
    camera_init(&camera);
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
                case SDL_KEYDOWN: {
                    if (event.key.repeat == 0) {
                        log_trace("Key was pressed");
                    } else {
                        log_trace("Key was pressed repeatedly");
                    }
                    enum EKey key = win32_input_remap_key(event.key.keysym.scancode);
                    win32_input_key_down[key] = true;
                    break;
                }
                case SDL_KEYUP: {
                    log_trace("Key was released");
                    enum EKey key = win32_input_remap_key(event.key.keysym.scancode);
                    win32_input_key_down[key] = false;
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    log_trace("Mouse button was pressed");
                    win32_input_button_down[event.button.button] = true;
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    log_trace("Mouse button was released");
                    win32_input_button_down[event.button.button] = false;
                    break;
                }
                case SDL_MOUSEMOTION: {
                    log_trace("Mouse moved (%d, %d) and is now at (%d, %d)", event.motion.xrel, event.motion.yrel, win32_input_mouse_x, win32_input_mouse_x);
                    break;
                }
                case SDL_MOUSEWHEEL: {
                    log_trace("Mouse wheel moved (%d, %d)", event.wheel.x, event.wheel.y);
                    break;
                }
                case SDL_WINDOWEVENT: {
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                        case SDL_WINDOWEVENT_RESIZED: {
                            window.width = event.window.data1;
                            window.height = event.window.data2;
                            window.is_framebuffer_resized = true;
                            log_trace("Window resized to (%d, %d)", event.window.data1, event.window.data2);
                            break;
                        }
                        case SDL_WINDOWEVENT_MINIMIZED: {
                            window.is_minimized = true;
                            log_trace("Window was minimized");
                            break;
                        }
                        case SDL_WINDOWEVENT_RESTORED: {
                            window.is_minimized = false;
                            log_trace("Window was restored");
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
        win32_input_update();
        camera_update(&camera, win32_input_key_down);
        vk_renderer_update(&camera);
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
