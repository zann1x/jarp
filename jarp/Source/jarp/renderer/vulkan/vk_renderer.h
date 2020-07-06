#ifndef VK_RENDERER_H
#define VK_RENDERER_H

#include "jarp/api_types.h"

bool vk_renderer_init(void* window, char* application_path);
void vk_renderer_shutdown(void);

void vk_renderer_update(void);
void vk_renderer_draw(void);

#endif
