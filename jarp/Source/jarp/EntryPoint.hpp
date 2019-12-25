#include "jarp/Application.h"
#include "jarp/Log.h"

extern jarp::Application* jarp::CreateApplication();

#if defined(JARP_PLATFORM_WINDOWS) || defined(JARP_PLATFORM_LINUX)

	int main(int argc, char** argv)
	{
		jarp::Log::Init();

		auto app = jarp::CreateApplication();
		app->Run();
		delete app;

		return 0;
	}

#endif
