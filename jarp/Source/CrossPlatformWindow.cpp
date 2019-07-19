#include "CrossPlatformWindow.h"

#include "SDL_syswm.h"

#include <iostream>

#define WIDTH 800
#define HEIGHT 600

CrossPlatformWindow::CrossPlatformWindow()
	: Width(800), Height(600), bShouldClose(false)
{
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		SDL_Log("Could not initialize SDL: %s", SDL_GetError());
		throw std::runtime_error("Could not initialize SDL!");
	}
}

CrossPlatformWindow::~CrossPlatformWindow()
{
	SDL_Quit();
}

void CrossPlatformWindow::Create()
{
	pWindow = SDL_CreateWindow("jarp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_VULKAN);
	if (!pWindow)
	{
		SDL_Log("Could not create a SDL renderer: %s", SDL_GetError());
		throw std::runtime_error("Could not create a SDL renderer!");
	}

	pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
	if (!pRenderer)
	{
		SDL_Log("Could not create a SDL renderer: %s", SDL_GetError());
		throw std::runtime_error("Could not create a SDL renderer!");
	}

	SDL_SetWindowResizable(pWindow, SDL_TRUE);

	//SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255);
}

void CrossPlatformWindow::Shutdown()
{
	SDL_DestroyWindow(pWindow);
	SDL_DestroyRenderer(pRenderer);
}

HINSTANCE CrossPlatformWindow::GetNativeInstanceHandle() const
{
	SDL_SysWMinfo SystemInfo;
	SDL_VERSION(&SystemInfo.version);
	SDL_GetWindowWMInfo(pWindow, &SystemInfo);
	return SystemInfo.info.win.hinstance;
}

HWND CrossPlatformWindow::GetNativeWindowHandle() const
{
	SDL_SysWMinfo SystemInfo;
	SDL_VERSION(&SystemInfo.version);
	SDL_GetWindowWMInfo(pWindow, &SystemInfo);
	return SystemInfo.info.win.window;
}

VkResult CrossPlatformWindow::CreateSurface(const VkInstance Instance, VkSurfaceKHR* SurfaceKHR) const
{
	if (!SDL_Vulkan_CreateSurface(pWindow, Instance, SurfaceKHR))
		return VK_ERROR_INITIALIZATION_FAILED;

	return VK_SUCCESS;
}

std::pair<int, int> CrossPlatformWindow::GetFramebufferSize()
{
	int Width, Height;
	SDL_Vulkan_GetDrawableSize(pWindow, &Width, &Height);
	return std::make_pair(Width, Height);
}

std::vector<const char*> CrossPlatformWindow::GetInstanceExtensions() const
{
	unsigned int Count;
	SDL_Vulkan_GetInstanceExtensions(pWindow, &Count, nullptr);

	std::vector<const char*> Extensions = { };
	size_t AdditionalExtensionCount = Extensions.size();
	Extensions.resize(AdditionalExtensionCount + Count);
	SDL_Vulkan_GetInstanceExtensions(pWindow, &Count, Extensions.data() + AdditionalExtensionCount);

	return Extensions;
}

bool CrossPlatformWindow::ShouldClose()
{
	return bShouldClose;
}

void CrossPlatformWindow::Update(uint32_t DeltaTime)
{
	SDL_Event Event;
	while (SDL_PollEvent(&Event))
	{
		switch (Event.type)
		{
		case SDL_QUIT:
			bShouldClose = true;
			break;
		case SDL_WINDOWEVENT_MINIMIZED:
			bIsWindowMinimized = true;
			break;
		case SDL_WINDOWEVENT_RESTORED:
			bIsWindowMinimized = false;
			break;
		case SDL_WINDOWEVENT_RESIZED:
			bIsFramebufferResized = true;
			break;
		}

		InputHandler.HandleInput(Event);
	}

	//SDL_RenderClear(pRenderer);
	//SDL_RenderPresent(pRenderer);
}
