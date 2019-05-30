#include "CrossPlatformWindow.h"

#include <vulkan/vulkan.h>
#include <iostream>

std::array<bool, 65536> CrossPlatformWindow::Keys = {};
float CrossPlatformWindow::MouseOffsetX = 0.0f;
float CrossPlatformWindow::MouseOffsetY = 0.0f;

CrossPlatformWindow::CrossPlatformWindow()
	: Width(800), Height(600), MouseX(Width / 2.0f), MouseY(Width / 2.0f)
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

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	CrossPlatformWindow::Keys[key] = (action != GLFW_RELEASE);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	CrossPlatformWindow* CPW = (CrossPlatformWindow*)glfwGetWindowUserPointer(window);
	CPW->MouseOffsetX = static_cast<float>(xpos) - CPW->MouseX;
	CPW->MouseOffsetY = static_cast<float>(ypos) - CPW->MouseY;
	CPW->MouseX = static_cast<float>(xpos);
	CPW->MouseY = static_cast<float>(ypos);
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
	glfwSetKeyCallback(pWindow, KeyCallback);
	glfwSetCursorPos(pWindow, MouseX, MouseY);
	glfwSetCursorPosCallback(pWindow, MouseCallback);
	glfwSetFramebufferSizeCallback(pWindow, FramebufferSizeCallback);
	glfwSetWindowIconifyCallback(pWindow, WindowIconifyCallback);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(pWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
