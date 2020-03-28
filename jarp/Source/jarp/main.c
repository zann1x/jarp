#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include <SDL.h>

#include "api_types.h"
#include "file.h"
#include "fps_counter.h"
#include "log.h"
#include "math.h"
#include "module.h"
#include "window.h"
#include "input/buttons.h"
#include "input/input.h"
#include "input/keys.h"

#include "Sandbox/sandbox.h"

static bool is_running;

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

int main(int argc, char** argv)
{
    log_trace("Tracing... %s", "asdf");
    log_debug("Debugging...");
    log_info("Infoing...");
    log_warn("Warning...");
    log_error("Erroring...");
    log_fatal("Fataling...");

    HMODULE sandbox_dll = LoadLibrary(TEXT("E:/VisualStudioProjects/jarp/Sandbox/Binaries/Debug-windows-x86_64/Sandbox/Sandbox.dll"));
    FARPROC load_sandbox = GetProcAddress(sandbox_dll, "load_sandbox");

    struct module_registry* reg = malloc(sizeof(struct module_registry));
    module_registry_init(reg);
    load_sandbox(reg);

    struct sandbox_api* sand = reg->get(SANDBOX_MODULE_NAME);
    log_info("Sandbox returning %d", sand->do_the_sand());

    char* buffer = file_read_asc("E:\\VisualStudioProjects\\jarp\\jarp\\Source\\jarp\\main.c");
    if (buffer != NULL)
    {
        log_info("%s", buffer);
        free(buffer);
    }

    Vec2f vec1 = { 1, 1 };
    Vec2f vec2 = { 2, 3 };
    Vec2f res = math_vec2f_add(vec1, vec2);

    window_init();
    fps_counter_init();

    is_running = true;
    while (is_running)
    {
        fps_counter_update(&window);
        handle_events();

        // render and update stuff
        input_update();
    }

    window_destroy();

    return EXIT_SUCCESS;
}
