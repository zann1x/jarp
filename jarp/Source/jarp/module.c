#include "module.h"

#include <stdlib.h>

void* loaded_modules;

ADD(add) {
	loaded_modules = interf;
}

REMOVE(remove) {
	loaded_modules = NULL;
}

GET(get) {
	return loaded_modules;
}

void module_registry_init(struct ModuleRegistry* reg) {
	reg->add = &add;
	reg->remove = &remove;
	reg->get = &get;
}
