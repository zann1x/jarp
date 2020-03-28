#include "module.h"

#include <stdlib.h>

void* loaded_modules;

void add(const char* name, void* interf)
{
	loaded_modules = interf;
}

void* get(const char* name)
{
	return loaded_modules;
}

void module_registry_init(struct module_registry* reg)
{
	reg->add = &add;
	reg->get = &get;
}
