#if defined(JARP_PLATFORM_WINDOWS)

#include "jarppch.h"
#include "WindowsWindow.h"

#include "jarp/Core.h"
#include "jarp/Events/ApplicationEvent.h"
#include "jarp/Events/EventBus.h"
#include "jarp/Events/KeyEvent.h"
#include "jarp/Events/MouseEvent.h"

#include <SDL_syswm.h>

namespace jarp {

	static bool bIsSDLInitialized = false;

	Window* Window::Create(const WindowProperties& properties)
	{
		return new WindowsWindow(properties);
	}

	WindowsWindow::WindowsWindow(const WindowProperties& properties)
	{
		m_Data.Title = properties.Title;
		m_Data.Width = properties.Width;
		m_Data.Height = properties.Height;

		if (!bIsSDLInitialized)
		{
			int SDLInitResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
			JARP_CORE_ASSERT(SDLInitResult == 0, "Could not initialize SDL!");
			bIsSDLInitialized = true;
		}

		m_Window = SDL_CreateWindow(m_Data.Title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_Data.Width, m_Data.Height, SDL_WINDOW_VULKAN);
		JARP_CORE_ASSERT(m_Window, "Could not create SDL window!");

		SDL_SetWindowResizable(m_Window, SDL_TRUE);
	}

	WindowsWindow::~WindowsWindow()
	{
		SDL_DestroyWindow(m_Window);
		SDL_Quit();
	}

	HINSTANCE WindowsWindow::GetNativeInstanceHandle() const
	{
		SDL_SysWMinfo systemInfo;
		SDL_VERSION(&systemInfo.version);
		SDL_GetWindowWMInfo(m_Window, &systemInfo);
		return systemInfo.info.win.hinstance;
	}

	HWND WindowsWindow::GetNativeWindowHandle() const
	{
		SDL_SysWMinfo systemInfo;
		SDL_VERSION(&systemInfo.version);
		SDL_GetWindowWMInfo(m_Window, &systemInfo);
		return systemInfo.info.win.window;
	}

	VkResult WindowsWindow::CreateSurface(const VkInstance instance, VkSurfaceKHR* surfaceKHR)
	{
		if (!SDL_Vulkan_CreateSurface(m_Window, instance, surfaceKHR))
			return VK_ERROR_INITIALIZATION_FAILED;

		return VK_SUCCESS;
	}

	std::pair<int, int> WindowsWindow::GetFramebufferSize() const
	{
		int width, height;
		SDL_Vulkan_GetDrawableSize(m_Window, &width, &height);
		return { width, height };
	}

	std::vector<const char*> WindowsWindow::GetInstanceExtensions() const
	{
		unsigned int count;
		SDL_Vulkan_GetInstanceExtensions(m_Window, &count, nullptr);

		std::vector<const char*> extensions = { };
		size_t additionalExtensionCount = extensions.size();
		extensions.resize(additionalExtensionCount + count);
		SDL_Vulkan_GetInstanceExtensions(m_Window, &count, extensions.data() + additionalExtensionCount);

		return extensions;
	}

	void WindowsWindow::Update(uint32_t deltaTime)
	{
		SDL_Event sdlEvent;
		while (SDL_PollEvent(&sdlEvent))
		{
			switch (sdlEvent.type)
			{
				case SDL_QUIT:
				{
					WindowClosedEvent event;
					EventBus::Get().Dispatch(event);
					break;
				}
				case SDL_WINDOWEVENT:
				{
					switch (sdlEvent.window.event)
					{
						case SDL_WINDOWEVENT_MINIMIZED:
						{
							m_bIsWindowMinimized = true;
							WindowMinimizedEvent event;
							EventBus::Get().Dispatch(event);
							break;
						}
						case SDL_WINDOWEVENT_RESTORED:
						{
							m_bIsWindowMinimized = false;
							WindowRestoredEvent event;
							EventBus::Get().Dispatch(event);
							break;
						}
						case SDL_WINDOWEVENT_SIZE_CHANGED:
						{
							m_bIsFramebufferResized = true;
							WindowResizedEvent event(sdlEvent.window.data1, sdlEvent.window.data2);
							EventBus::Get().Dispatch(event);
							break;
						}
					}
				}
				case SDL_MOUSEMOTION:
				{
					int x, y;
					SDL_GetMouseState(&x, &y);
					MouseMovedEvent event(x, y);
					EventBus::Get().Dispatch(event);
					break;
				}
				case SDL_KEYDOWN:
				{
					JARP_CORE_INFO("Key pressed: {0}", sdlEvent.key.keysym.sym);
					KeyPressedEvent event(sdlEvent.key.keysym.sym);
					EventBus::Get().Dispatch(event);
					break;
				}
			}
		}
	}

}

#endif
