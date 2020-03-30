//#include <stdio.h>
//#include <stdlib.h>
//#include <Windows.h>
//
//#include <SDL.h>
//
//#include "api_types.h"
//#include "file.h"
//#include "log.h"
//#include "math.h"
//#include "module.h"
//#include "platform.h"
//#include "window.h"
//#include "input/buttons.h"
//#include "input/input.h"
//#include "input/keys.h"
//
//#include "Sandbox/sandbox.h"
//
//bool is_running;
//
//void handle_events(void)
//{
//    SDL_Event event;
//    while (SDL_PollEvent(&event) != 0)
//    {
//        switch (event.type)
//        {
//            case SDL_QUIT:
//            {
//                is_running = false;
//                break;
//            }
//            case SDL_KEYDOWN:
//            case SDL_KEYUP:
//            case SDL_MOUSEBUTTONDOWN:
//            case SDL_MOUSEBUTTONUP:
//            case SDL_MOUSEMOTION:
//            case SDL_MOUSEWHEEL:
//            {
//                input_event(&event);
//                break;
//            }
//            case SDL_WINDOWEVENT:
//            {
//                window_event(&event);
//                break;
//            }
//        }
//    }
//}
//
//const char* full_game_dll_path = "E:/code/jarp/Game/Binaries/Debug-windows-x86_64/Game.dll";
//const char* full_game_temp_dll_path = "E:/code/jarp/Game/Binaries/Debug-windows-x86_64/Game_temp.dll";
//const char* lock_full_path = "E:/code/jarp/Game/Binaries/Debug-windows-x86_64/lock.tmp";
//struct GameCode
//{
//    HMODULE dll;
//    FILETIME last_dll_write_time;
//
//    GameUpdateAndRender* update_and_render;
//};
//
//const char* full_sandbox_dll_path = "E:/code/jarp/Sandbox/Binaries/Debug-windows-x86_64/Sandbox.dll";
//const char* full_sandbox_temp_dll_path = "E:/code/jarp/Sandbox/Binaries/Debug-windows-x86_64/Sandbox_temp.dll";
//struct SandboxCode
//{
//    FILETIME last_dll_write_time;
//    void* sandbox_dll;
//
//    struct sandbox_api* sandbox_api;
//};
//
//FILETIME win32_get_last_write_time(const char* filename)
//{
//    FILETIME last_write_time = { 0 };
//    WIN32_FILE_ATTRIBUTE_DATA data;
//
//    BOOL success = GetFileAttributesExA(filename, GetFileExInfoStandard, &data);
//    if (success)
//    {
//        last_write_time = data.ftLastWriteTime;
//    }
//
//    return last_write_time;
//}
//
//void load_game_code(struct GameCode* game_code)
//{
//    WIN32_FILE_ATTRIBUTE_DATA ignored;
//    if (!GetFileAttributesExA(lock_full_path, GetFileExInfoStandard, &ignored))
//    {
//        log_trace("Loading game code");
//
//        game_code->last_dll_write_time = win32_get_last_write_time(full_game_dll_path);
//
//        if (!CopyFileA(full_game_dll_path, full_game_temp_dll_path, FALSE))
//            log_error("Copying the DLL did not work: %d", GetLastError());
//        else
//            log_info("Copied DLL");
//
//        game_code->dll = LoadLibraryA(full_game_temp_dll_path);
//        if (!game_code->dll)
//            log_error("Failed loading the DLL");
//        else
//        {
//            game_code->update_and_render = (GameUpdateAndRender*)GetProcAddress(game_code->dll, "game_update_and_render");
//
//            if (!game_code->update_and_render)
//                log_error("Failed loading DLL function");
//        }
//    }
//    else
//    {
//        log_trace("Lock file present, cannot reload game code");
//    }
//}
//
//void unload_game_code(struct GameCode* game_code)
//{
//    if (game_code->dll)
//    {
//        log_trace("Unloading game code");
//
//        if (!FreeLibrary(game_code->dll))
//            log_error("Failed unloading the DLL: %d", GetLastError());
//        game_code->dll = 0;
//    }
//
//    game_code->update_and_render = NULL;
//}
//
////struct SandboxCode load_sandbox_code(struct ModuleRegistry* reg)
////{
////    log_info("loading sandbox code");
////
////    const char* source_dll_name = full_sandbox_dll_path;
////    const char* temp_dll_name = full_sandbox_temp_dll_path;
////
////    struct SandboxCode sandbox_code;
////    CopyFileA(source_dll_name, temp_dll_name, FALSE);
////    sandbox_code.sandbox_dll = SDL_LoadObject(temp_dll_name);
////    sandbox_code.last_dll_write_time = win32_get_last_write_time(source_dll_name);
////    LoadSandbox* load_sandbox = (LoadSandbox*)SDL_LoadFunction(sandbox_code.sandbox_dll, "load_sandbox");
////    load_sandbox(reg);
////    sandbox_code.sandbox_api = reg->get(SANDBOX_MODULE_NAME);
////
////    return sandbox_code;
////}
//
////void unload_sandbox_code(struct SandboxCode* sand_code, struct ModuleRegistry* reg)
////{
////    UnloadSandbox* unload_sandbox = (UnloadSandbox*)SDL_LoadFunction(sand_code->sandbox_dll, "unload_sandbox");
////    unload_sandbox(reg);
////    SDL_UnloadObject(sand_code->sandbox_dll);
////    // TODO: null all values of sand_code
////}
//
//PLATFORM_TEST(win32_test)
//{
//    return 1;
//}
//
//int main(int argc, char** argv)
//{
//    //log_trace("Tracing... %s", "asdf");
//    //log_debug("Debugging...");
//    //log_info("Infoing...");
//    //log_warn("Warning...");
//    //log_error("Erroring...");
//    //log_fatal("Fataling...");
//
//    struct ModuleRegistry reg;
//    module_registry_init(&reg);
//    //struct SandboxCode sand_code = load_sandbox_code(&reg);
//
//    struct GameCode game_code;
//    load_game_code(&game_code);
//
//    struct PlatformAPI platform_api;
//    platform_api.test = win32_test;
//
//    struct GameMemory game_memory;
//    game_memory.platform_api = platform_api;
//
//    //struct FileContent file_content = file_read_asc("E:\\VisualStudioProjects\\jarp\\jarp\\Source\\jarp\\main.c");
//    //if (file_content.buffer != NULL)
//    //{
//    //    log_info("%s", file_content.buffer);
//    //    free(file_content.buffer);
//    //}
//
//    //Vec2f vec1 = { 1, 1 };
//    //Vec2f vec2 = { 2, 3 };
//    //Vec2f res = math_vec2f_add(vec1, vec2);
//
//    window_init();
//
//    uint32_t current_fps_time = SDL_GetTicks();
//    uint32_t last_fps_time = current_fps_time;
//    uint32_t frames = 0;
//
//    is_running = true;
//    while (is_running)
//    {
//        handle_events();
//
//        // render and update stuff
//        input_update();
//
//#if _DEBUG
//        // reload game code if a change is detected
//        bool game_code_reload_needed = false;
//        FILETIME new_game_dll_write_time = win32_get_last_write_time(full_game_dll_path);
//        game_code_reload_needed = CompareFileTime(&new_game_dll_write_time, &game_code.last_dll_write_time) != 0;
//        if (game_code_reload_needed)
//        {
//            unload_game_code(&game_code);
//            load_game_code(&game_code);
//        }
//
//        //// reload sandbox code if a change is detected
//        //bool sandbox_code_reload_needed = false;
//        //FILETIME new_dll_write_time = win32_get_last_write_time(full_sandbox_dll_path);
//        //sandbox_code_reload_needed = CompareFileTime(&new_dll_write_time, &sand_code.last_dll_write_time) != 0;
//        //if (sandbox_code_reload_needed)
//        //{
//        //    unload_sandbox_code(&sand_code, &reg);
//        //    sand_code = load_sandbox_code(&reg);
//        //}
//#endif
//
//        // frame time
//        current_fps_time = SDL_GetTicks();
//        ++frames;
//        if (current_fps_time > last_fps_time + 1000)
//        {
//            char buffer[32];
//            sprintf(buffer, "%s - %d fps", window.title, frames);
//            window_set_display_title(buffer);
//
//            game_code.update_and_render(&game_memory);
//            //log_info("Sandbox returning %d", sand_code.sandbox_api->do_the_sand());
//
//            last_fps_time = current_fps_time;
//            frames = 0;
//        }
//    }
//
//    window_destroy();
//
//    //unload_sandbox(&reg);
//
//    return EXIT_SUCCESS;
//}
