#include "jarppch.h"
#include "VulkanSurface.h"

#include "jarp/Application.h"
#include "jarp/Window.h"

#include "Platform/Linux/LinuxWindow.h"
#include "Platform/VulkanRHI/VulkanRendererAPI.h"
#include "Platform/VulkanRHI/VulkanUtils.hpp"
#include "Platform/Windows/WindowsWindow.h"

namespace jarp {

	VulkanSurface::VulkanSurface()
	{
	}

	VulkanSurface::~VulkanSurface()
	{
	}

	void VulkanSurface::CreateSurface()
	{
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		WindowsWindow& win32Window = dynamic_cast<WindowsWindow&>(Application::Get().GetWindow());

		VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfoKHR = {};
		win32SurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		win32SurfaceCreateInfoKHR.pNext = nullptr;
		win32SurfaceCreateInfoKHR.flags = 0;
		win32SurfaceCreateInfoKHR.hinstance = win32Window.GetNativeInstanceHandle();
		win32SurfaceCreateInfoKHR.hwnd = win32Window.GetNativeWindowHandle();

		VK_ASSERT(vkCreateWin32SurfaceKHR(VulkanRendererAPI::s_Instance->GetHandle(), &win32SurfaceCreateInfoKHR, nullptr, &m_SurfaceKHR));
#elif defined(VK_USE_PLATFORM_XCB_KHR)
		LinuxWindow& xcbWindow = dynamic_cast<LinuxWindow&>(Application::Get().GetWindow());

		VkXcbSurfaceCreateInfoKHR xcbSurfaceCreateInfoKHR = {};
		xcbSurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		xcbSurfaceCreateInfoKHR.pNext = nullptr;
		xcbSurfaceCreateInfoKHR.flags = 0;
		xcbSurfaceCreateInfoKHR.connection = xcbWindow.GetNativeConnectionHandle();
		xcbSurfaceCreateInfoKHR.window = xcbWindow.GetNativeWindowHandle();

		vkCreateXcbSurfaceKHR(VulkanRendererAPI::s_Instance->GetHandle(), &xcbSurfaceCreateInfoKHR, nullptr, &m_SurfaceKHR);
#else
#error UNSUPPORTED PLATFORM
#endif
	}

	void VulkanSurface::Destroy()
	{
		vkDestroySurfaceKHR(VulkanRendererAPI::s_Instance->GetHandle(), m_SurfaceKHR, nullptr);
	}

}
