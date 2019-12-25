// #pragma once

// extern jarp::Application* jarp::CreateApplication();

#include "jarp/Application.h"
#include "jarp/Log.h"

#if defined(JARP_PLATFORM_WINDOWS)

	int main(int argc, char** argv)
	{
		auto app = jarp::CreateApplication();
		app->Run();
		delete app;

		return 0;
	}

#elif defined(JARP_PLATFORM_LINUX)

	int main()
	{
		auto app = new jarp::Application();

		JARP_CORE_WARN("Platform not completely supported yet");

		app->Run();
		delete app;

		return 0;
	}

#else
	#error UNSUPPORTED PLATFORM
#endif
