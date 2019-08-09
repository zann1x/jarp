#pragma once

extern jarp::Application* jarp::CreateApplication();

#if defined(JARP_PLATFORM_WINDOWS)
int main(int argc, char** argv)
{
	auto App = jarp::CreateApplication();
	App->Run();
	delete App;

	return 0;
}
#else
#error UNSUPPORTED PLATFORM
#endif
