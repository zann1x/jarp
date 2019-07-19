#pragma once

#include <vulkan/vulkan.h>

#include "SDL.h"
#include "SDL_vulkan.h"

#include <array>
#include <utility>
#include <vector>

#include "jarp/Input/InputHandler.h"

class WindowsWindow
{
public:
	WindowsWindow();
	~WindowsWindow();

	void Create();
	void Shutdown();

	inline SDL_Window* GetHandle() { return pWindow; }

	HINSTANCE GetNativeInstanceHandle() const;
	HWND GetNativeWindowHandle() const;

	VkResult CreateSurface(const VkInstance Instance, VkSurfaceKHR* SurfaceKHR) const;
	std::vector<const char*> GetInstanceExtensions() const;

	// Returns the framebuffer width as the first parameter and height as the second
	std::pair<int, int> GetFramebufferSize();
	inline int GetWidth() { return Width; }
	inline int GetHeight() { return Height; }

	inline bool IsIconified() { return bIsWindowMinimized; }
	inline bool IsFramebufferResized() { return bIsFramebufferResized; }
	void SetFramebufferResized(bool FramebufferResized) { bIsFramebufferResized = FramebufferResized; }

	bool ShouldClose();
	void Update(uint32_t DeltaTime);

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
