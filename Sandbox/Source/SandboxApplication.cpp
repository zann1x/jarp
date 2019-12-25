#include "jarp.h"
#include "jarp/EntryPoint.hpp"

class SandboxApplication : public jarp::Application
{
public:
	SandboxApplication()
	{
		JARP_INFO("Created Sandbox application");
	}

	~SandboxApplication() { }
};

jarp::Application* jarp::CreateApplication()
{
	return new SandboxApplication();
}
