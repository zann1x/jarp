#pragma once

#define API __declspec(dllexport)

#define SANDBOX_MODULE_NAME "sandbox_module"

API void load_sandbox(struct module_registry* reg);

struct sandbox_api
{
	int (*do_the_sand)();
};
