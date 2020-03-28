#pragma once

struct module_registry
{
    void (*add)(const char* name, void* interf);
    void* (*get)(const char* name);
};

void module_registry_init();
