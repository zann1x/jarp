#ifndef MODULE_H
#define MODULE_H

#define ADD(name) void name(const char* name, void* interf)
typedef ADD(Add);

#define REMOVE(name) void name(const char* name)
typedef REMOVE(Remove);

#define GET(name) void* name(const char* name)
typedef GET(Get);

struct ModuleRegistry {
    Add* add;
    Remove* remove;
    Get* get;
};

void module_registry_init(struct ModuleRegistry* reg);

#endif
