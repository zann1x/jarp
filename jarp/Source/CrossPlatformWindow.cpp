#include "CrossPlatformWindow.h"

#include <iostream>

#define WIDTH 800
#define HEIGHT 600

CrossPlatformWindow::CrossPlatformWindow()
	: Width(800), Height(600), bShouldClose(false)
{
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
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
	pRenderer = SDL_CreateRenderer(pWindow, -1, 0);

	//SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255);
}

void CrossPlatformWindow::Shutdown()
{
	SDL_DestroyWindow(pWindow);
	SDL_DestroyRenderer(pRenderer);
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

std::vector<const char*> CrossPlatformWindow::GetInstanceExtensions()
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

void CrossPlatformWindow::Update()
{
	SDL_Event Event;
	SDL_PollEvent(&Event);
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

	//SDL_RenderClear(pRenderer);
	//SDL_RenderPresent(pRenderer);
}
