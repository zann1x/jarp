#ifdef JARP_PLATFORM_LINUX

#include "jarppch.h"

#include "LinuxWindow.h"

#include "jarp/Core.h"

namespace jarp {

    static bool bIsSDLInitialized = false;

    Window* Window::Create(const WindowProperties& properties)
	{
		return new LinuxWindow(properties);
	}

    LinuxWindow::LinuxWindow(const WindowProperties& Properties)
    {
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

}

#endif
