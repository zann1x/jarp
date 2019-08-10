#include "jarppch.h"
#include "WindowsWindow.h"

#include "jarp/Core.h"
#include "jarp/Events/ApplicationEvent.h"
#include "jarp/Events/EventBus.h"
#include "jarp/Events/KeyEvent.h"
#include "jarp/Events/MouseEvent.h"

#include "SDL_syswm.h"

namespace jarp {

	Window* Window::Create(const WindowProperties& Properties)
	{
		return new WindowsWindow(Properties);
	}

	static bool IsSDLInitialized = false;

	WindowsWindow::WindowsWindow(const WindowProperties& Properties)
	{
		Data.Title = Properties.Title;
		Data.Width = Properties.Width;
		Data.Height = Properties.Height;

		if (!IsSDLInitialized)
		{
			int SDLInitResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
			JARP_CORE_ASSERT(SDLInitResult == 0, "Could not initialize SDL!");
			IsSDLInitialized = true;
		}

		pWindow = SDL_CreateWindow(Data.Title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Data.Width, Data.Height, SDL_WINDOW_VULKAN);
		JARP_CORE_ASSERT(pWindow, "Could not create SDL window!");

		SDL_SetWindowResizable(pWindow, SDL_TRUE);
	}

	WindowsWindow::~WindowsWindow()
	{
		SDL_DestroyWindow(pWindow);
		SDL_Quit();
	}

	HINSTANCE WindowsWindow::GetNativeInstanceHandle() const
	{
		SDL_SysWMinfo SystemInfo;
		SDL_VERSION(&SystemInfo.version);
		SDL_GetWindowWMInfo(pWindow, &SystemInfo);
		return SystemInfo.info.win.hinstance;
	}

	HWND WindowsWindow::GetNativeWindowHandle() const
	{
		SDL_SysWMinfo SystemInfo;
		SDL_VERSION(&SystemInfo.version);
		SDL_GetWindowWMInfo(pWindow, &SystemInfo);
		return SystemInfo.info.win.window;
	}

	VkResult WindowsWindow::CreateSurface(const VkInstance Instance, VkSurfaceKHR* SurfaceKHR) const
	{
		if (!SDL_Vulkan_CreateSurface(pWindow, Instance, SurfaceKHR))
			return VK_ERROR_INITIALIZATION_FAILED;

		return VK_SUCCESS;
	}

	std::pair<int, int> WindowsWindow::GetFramebufferSize() const
	{
		int Width, Height;
		SDL_Vulkan_GetDrawableSize(pWindow, &Width, &Height);
		return { Width, Height };
	}

	std::vector<const char*> WindowsWindow::GetInstanceExtensions() const
	{
		unsigned int Count;
		SDL_Vulkan_GetInstanceExtensions(pWindow, &Count, nullptr);

		std::vector<const char*> Extensions = { };
		size_t AdditionalExtensionCount = Extensions.size();
		Extensions.resize(AdditionalExtensionCount + Count);
		SDL_Vulkan_GetInstanceExtensions(pWindow, &Count, Extensions.data() + AdditionalExtensionCount);

		return Extensions;
	}

	void WindowsWindow::Update(uint32_t DeltaTime)
	{
		SDL_Event Event;
		while (SDL_PollEvent(&Event))
		{
			switch (Event.type)
			{
				case SDL_QUIT:
				{
					WindowClosedEvent E(&Event);
					EventBus::Get().Dispatch(E);
					break;
				}
				case SDL_WINDOWEVENT_MINIMIZED:
				{
					bIsWindowMinimized = true;
					WindowMinimizedEvent E(&Event);
					EventBus::Get().Dispatch(E);
					break;
				}
				case SDL_WINDOWEVENT_RESTORED:
				{
					bIsWindowMinimized = false;
					break;
				}
				case SDL_WINDOWEVENT_RESIZED:
				{
					bIsFramebufferResized = true;
					int Width, Height;
					SDL_GetWindowSize(pWindow, &Width, &Height);
					WindowResizedEvent E(&Event, Width, Height);
					EventBus::Get().Dispatch(E);
					break;
				}
				case SDL_MOUSEMOTION:
				{
					int XPos, YPos;
					SDL_GetMouseState(&XPos, &YPos);
					MouseMovedEvent E(&Event, XPos, YPos);
					EventBus::Get().Dispatch(E);
					break;
				}
				case SDL_KEYDOWN:
				{
					JARP_CORE_INFO("Key pressed: {0}", Event.key.keysym.sym);
					KeyPressedEvent E(&Event, Event.key.keysym.sym);
					EventBus::Get().Dispatch(E);
					break;
				}
			}
		}
	}

}
