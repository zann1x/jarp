#pragma once

#define VK_USE_PLATFROM_WIN32_KHR
#include <vulkan/vulkan.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <array>
#include <utility>
#include <vector>

#include "Input/InputHandler.h"

class CrossPlatformWindow
{
public:
	CrossPlatformWindow();
	~CrossPlatformWindow();

	void Create();
	void Shutdown();

	inline SDL_Window* GetHandle() { return pWindow; }

	VkResult CreateSurface(const VkInstance Instance, VkSurfaceKHR* SurfaceKHR) const;
	std::vector<const char*> GetInstanceExtensions();

	// Returns the framebuffer width as the first parameter and height as the second
	std::pair<int, int> GetFramebufferSize();
	inline int GetWidth() { return Width; }
	inline int GetHeight() { return Height; }

	inline bool IsIconified() { return bIsWindowMinimized; }
	inline bool IsFramebufferResized() { return bIsFramebufferResized; }
	void SetFramebufferResized(bool FramebufferResized) { bIsFramebufferResized = FramebufferResized; }

	bool ShouldClose();
	void Update();

private:
	int Width;
	int Height;

	SDL_Window* pWindow;
	SDL_Renderer* pRenderer;
	bool bIsFramebufferResized;
	bool bIsWindowMinimized;
	bool bShouldClose;

	InputHandler InputHandler;
};
