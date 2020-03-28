#include "sandbox.h"

#include "module.h"

#include <stdlib.h>

struct sandbox_api* sand;

int do_the_sand(void)
{
    return 42;
}

void load_sandbox(struct module_registry* reg)
{
    sand = malloc(sizeof(struct sandbox_api));
    sand->do_the_sand = &do_the_sand;
    reg->add(SANDBOX_MODULE_NAME, sand);
}
