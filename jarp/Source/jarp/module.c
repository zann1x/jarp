static void* loaded_modules;

ADD(module_add) {
	loaded_modules = interf;
}

REMOVE(module_remove) {
	loaded_modules = NULL;
}

GET(module_get) {
	return loaded_modules;
}

void module_registry_init(struct ModuleRegistry* reg) {
	reg->add = &module_add;
	reg->remove = &module_remove;
	reg->get = &module_get;
}
