#ifndef SANDBOX_H
#define SANDBOX_H

#include "jarp/api_types.h"

#define SANDBOX_MODULE_NAME "sandbox_module"

#define LOAD_SANDBOX(name) void name(struct ModuleRegistry* reg)
typedef LOAD_SANDBOX(LoadSandbox);
API LOAD_SANDBOX(load_sandbox);

#define UNLOAD_SANDBOX(name) void name(struct ModuleRegistry* reg)
typedef UNLOAD_SANDBOX(UnloadSandbox);
API UNLOAD_SANDBOX(unload_sandbox);

#define DO_THE_SAND(name) int name(void)
typedef DO_THE_SAND(DoTheSand);

struct SandboxAPI {
    DoTheSand* do_the_sand;
};

#endif
