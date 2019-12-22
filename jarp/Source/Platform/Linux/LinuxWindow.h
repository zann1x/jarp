#pragma once

#ifdef JARP_PLATFORM_LINUX

#include "jarp/Window.h"

#include <volk.h>

#include <SDL.h>
#include <SDL_vulkan.h>

namespace jarp {

    class LinuxWindow : public Window
    {
    public:
        LinuxWindow(const WindowProperties& Properties = WindowProperties());
        virtual ~LinuxWindow();

        // Linux XCB specific //
		xcb_connection_t* GetNativeConnectionHandle() const;
		xcb_window_t GetNativeWindowHandle() const;
		// //

        virtual void Update(uint32_t deltaTime) override;

		virtual std::pair<int, int> GetFramebufferSize() const override;
		virtual int GetWidth() const override;
		virtual int GetHeight() const override;
		virtual const char* GetTitle() const override;
		virtual void* GetNativeWindow() const override;

		virtual bool IsMinimized() const override;

    private:
        SDL_Window* m_Window;
    };

}

#endif
