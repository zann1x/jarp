#include <jarp.h>

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
