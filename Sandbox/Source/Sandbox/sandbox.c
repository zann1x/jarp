#include "sandbox.h"

#include "jarp/module.h"

#include <stdlib.h>

struct SandboxAPI* sand;

DO_THE_SAND(do_the_sand) {
    return 43;
}

LOAD_SANDBOX(load_sandbox) {
    sand = malloc(sizeof(struct SandboxAPI));
    sand->do_the_sand = &do_the_sand;
    reg->add(SANDBOX_MODULE_NAME, sand);
}

UNLOAD_SANDBOX(unload_sandbox) {
    reg->remove(SANDBOX_MODULE_NAME);
    free(sand);
}
