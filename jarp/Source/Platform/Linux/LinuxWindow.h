#pragma once

#ifdef JARP_PLATFORM_LINUX

#include "jarp/Window.h"

#include <volk.h>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <xcb/xcb.h>

namespace jarp {

    class LinuxWindow : public Window
    {
    public:
        LinuxWindow(const WindowProperties& Properties = WindowProperties());
        virtual ~LinuxWindow();

#if defined(VK_USE_PLATFORM_XCB_KHR)
		xcb_connection_t* GetNativeConnectionHandle() const;
		xcb_window_t GetNativeWindowHandle() const;
#endif

        virtual void Update(uint32_t deltaTime) override;

		virtual std::pair<int, int> GetFramebufferSize() const override;
		virtual int GetWidth() const override { return m_Data.Width; };
		virtual int GetHeight() const override { return m_Data.Height; };
		virtual const char* GetTitle() const override { return m_Data.Title; };
		virtual void* GetNativeWindow() const override { return m_Window; }

		virtual bool IsMinimized() const override { return m_bIsWindowMinimized; }
		virtual inline bool IsFramebufferResized() const override { return m_bIsFramebufferResized; }
		virtual void SetFramebufferResized(bool bIsFramebufferResized) override { m_bIsFramebufferResized = bIsFramebufferResized; }

    private:
        SDL_Window* m_Window;
        bool m_bIsFramebufferResized = false;
        bool m_bIsWindowMinimized = false;

        struct SWindowData
		{
			const char* Title;
			int Width;
			int Height;
		} m_Data;
    };

}

#endif
