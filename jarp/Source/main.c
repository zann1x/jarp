#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "file.h"
#include "fps_counter.h"
#include "log.h"
#include "math.h"
#include "window.h"
#include "input/buttons.h"
#include "input/input.h"
#include "input/keys.h"

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

    char* buffer = file_read_asc("E:\\VisualStudioProjects\\jarp\\jarp\\Source\\main.c");
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
