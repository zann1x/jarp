#ifdef JARP_PLATFORM_LINUX

#include "LinuxWindow.h"

#include "jarp/Core.h"
#include "jarp/Events/ApplicationEvent.h"
#include "jarp/Events/EventBus.h"
#include "jarp/Events/KeyEvent.h"
#include "jarp/Events/MouseEvent.h"

#include <SDL_syswm.h>
#include <X11/Xlib-xcb.h>

namespace jarp {

    static bool bIsSDLInitialized = false;

    Window* Window::Create(const WindowProperties& properties)
	{
		return new LinuxWindow(properties);
	}

    LinuxWindow::LinuxWindow(const WindowProperties& Properties)
    {
		m_Data.Title = Properties.Title;
		m_Data.Width = Properties.Width;
		m_Data.Height = Properties.Height;

        if (!bIsSDLInitialized)
		{
			int SDLInitResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
			JARP_CORE_ASSERT(SDLInitResult == 0, "Could not initialize SDL!");
			bIsSDLInitialized = true;
		}

        m_Window = SDL_CreateWindow("linux", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Properties.Width, Properties.Height, SDL_WINDOW_VULKAN);
		JARP_CORE_ASSERT(m_Window, "Could not create SDL window!");

		SDL_SetWindowResizable(m_Window, SDL_TRUE);
    }
    
    LinuxWindow::~LinuxWindow()
    {
		SDL_DestroyWindow(m_Window);
		SDL_Quit();
    }

#if defined(VK_USE_PLATFORM_XCB_KHR)
	xcb_connection_t* LinuxWindow::GetNativeConnectionHandle() const
	{
		SDL_SysWMinfo systemInfo;
		SDL_VERSION(&systemInfo.version);
		SDL_GetWindowWMInfo(m_Window, &systemInfo);
		return XGetXCBConnection(systemInfo.info.x11.display);
	}

	xcb_window_t LinuxWindow::GetNativeWindowHandle() const
	{
		SDL_SysWMinfo systemInfo;
		SDL_VERSION(&systemInfo.version);
		SDL_GetWindowWMInfo(m_Window, &systemInfo);
		return systemInfo.info.x11.window;
	}
#endif

	void LinuxWindow::Update(uint32_t deltaTime)
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

	std::pair<int, int> LinuxWindow::GetFramebufferSize() const
	{
		int width, height;
		SDL_Vulkan_GetDrawableSize(m_Window, &width, &height);
		return { width, height };
	}

}

#endif
