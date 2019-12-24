#pragma once

extern jarp::Application* jarp::CreateApplication();

#if defined(JARP_PLATFORM_WINDOWS)

	int main(int argc, char** argv)
	{
		auto app = jarp::CreateApplication();
		app->Run();
		delete app;

		return 0;
	}

#elif defined(JARP_PLATFORM_LINUX)

	int main(int argc, char** argv)
	{
		JARP_CORE_ERROR("Platform yet to be supported");

		return 0;
	}

#else
	#error UNSUPPORTED PLATFORM
#endif
