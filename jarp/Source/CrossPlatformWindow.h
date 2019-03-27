#pragma once

#define VK_USE_PLATFROM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <utility>

class CrossPlatformWindow
{
public:
	CrossPlatformWindow();
	~CrossPlatformWindow();

	void StartGlfwWindow();
	void ShutdownGlfw();

	inline GLFWwindow* GetHandle() { return pWindow; }

	VkResult CreateSurface(const VkInstance Instance, VkSurfaceKHR* SurfaceKHR) const;

	// Returns the framebuffer width as the first parameter and height as the second
	std::pair<int, int> GetFramebufferSize();
	inline int GetWidth() { return Width; }
	inline int GetHeight() { return Height; }

	inline bool IsIconified() { return bIsWindowIconified; }
	inline bool IsFramebufferResized() { return bIsFramebufferResized; }
	void SetFramebufferResized(bool FramebufferResized) { bIsFramebufferResized = FramebufferResized; }

	bool ShouldClose();
	void PollEvents();

private:
	int Width;
	int Height;

	GLFWwindow* pWindow;
	bool bIsFramebufferResized;
	bool bIsWindowIconified;

	friend static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	friend static void WindowIconifyCallback(GLFWwindow* window, int iconified);
};
