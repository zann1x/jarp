#include "CrossPlatformWindow.h"

#include <vulkan/vulkan.h>
#include <iostream>

CrossPlatformWindow::CrossPlatformWindow()
	: Width(800), Height(600)
{
	if (!glfwInit())
		throw std::runtime_error("Could not initialize GLFW!");
}

CrossPlatformWindow::~CrossPlatformWindow()
{
	glfwTerminate();
}

void ErrorCallback(int error, const char* description)
{
	std::cerr << "Error " << error << ": " << description << std::endl;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	CrossPlatformWindow* CPW = (CrossPlatformWindow*)glfwGetWindowUserPointer(window);
	CPW->bIsFramebufferResized = true;
}

void WindowIconifyCallback(GLFWwindow* window, int iconified)
{
	CrossPlatformWindow* CPW = (CrossPlatformWindow*)glfwGetWindowUserPointer(window);
	if (iconified)
	{
		CPW->bIsWindowIconified = true;
	}
	else
	{
		CPW->bIsWindowIconified = false;
	}
}

void CrossPlatformWindow::StartGlfwWindow()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	pWindow = glfwCreateWindow(Width, Height, "jarp", nullptr, nullptr);

	glfwSetWindowUserPointer(pWindow, this);
	glfwSetErrorCallback(ErrorCallback);
	glfwSetFramebufferSizeCallback(pWindow, FramebufferSizeCallback);
	glfwSetWindowIconifyCallback(pWindow, WindowIconifyCallback);
}

void CrossPlatformWindow::ShutdownGlfw()
{
	glfwDestroyWindow(pWindow);
}

VkResult CrossPlatformWindow::CreateSurface(const VkInstance Instance, VkSurfaceKHR* SurfaceKHR) const
{
	return glfwCreateWindowSurface(Instance, pWindow, nullptr, SurfaceKHR);
}

std::pair<int, int> CrossPlatformWindow::GetFramebufferSize()
{
	int Width, Height;
	glfwGetFramebufferSize(pWindow, &Width, &Height);
	return std::make_pair(Width, Height);
}

bool CrossPlatformWindow::ShouldClose()
{
	return glfwWindowShouldClose(pWindow);
}

void CrossPlatformWindow::PollEvents()
{
	glfwPollEvents();
}
