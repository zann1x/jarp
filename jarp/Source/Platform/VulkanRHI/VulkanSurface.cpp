#include "jarppch.h"
#include "VulkanSurface.h"

#include "jarp/Application.h"
#include "jarp/Window.h"

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
		WindowsWindow& Win32 = dynamic_cast<WindowsWindow&>(Application::Get().GetWindow());

		VkWin32SurfaceCreateInfoKHR Win32SurfaceCreateInfoKHR = {};
		Win32SurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		Win32SurfaceCreateInfoKHR.pNext = nullptr;
		Win32SurfaceCreateInfoKHR.flags = 0;
		Win32SurfaceCreateInfoKHR.hinstance = Win32.GetNativeInstanceHandle();
		Win32SurfaceCreateInfoKHR.hwnd = Win32.GetNativeWindowHandle();

		VK_ASSERT(vkCreateWin32SurfaceKHR(VulkanRendererAPI::pInstance->GetHandle(), &Win32SurfaceCreateInfoKHR, nullptr, &SurfaceKHR));
#else
#error UNSUPPORTED PLATFORM
#endif
	}

	void VulkanSurface::Destroy()
	{
		vkDestroySurfaceKHR(VulkanRendererAPI::pInstance->GetHandle(), SurfaceKHR, nullptr);
	}

}
