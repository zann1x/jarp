#pragma once

#if defined(JARP_PLATFORM_WINDOWS)

#include "jarp/Window.h"

#include <volk.h>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <utility>
#include <vector>

namespace jarp {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties& Properties = WindowProperties());
		virtual ~WindowsWindow();

		inline SDL_Window* GetHandle() { return m_Window; }

		// Windows specific //
		HINSTANCE GetNativeInstanceHandle() const;
		HWND GetNativeWindowHandle() const;
		// //

		VkResult CreateSurface(const VkInstance instance, VkSurfaceKHR* surfaceKHR);
		std::vector<const char*> GetInstanceExtensions() const;

		// Returns the framebuffer width as the first parameter and height as the second
		virtual std::pair<int, int> GetFramebufferSize() const override;
		virtual inline int GetWidth() const override { return m_Data.Width; }
		virtual inline int GetHeight() const override { return m_Data.Height; }
		virtual inline const char* GetTitle() const override { return m_Data.Title; }
		virtual inline void* GetNativeWindow() const override { return m_Window; };

		virtual inline bool IsMinimized() const override { return m_bIsWindowMinimized; }
		virtual inline bool IsFramebufferResized() const override { return m_bIsFramebufferResized; }
		virtual void SetFramebufferResized(bool bIsFramebufferResized) override { m_bIsFramebufferResized = bIsFramebufferResized; }

		virtual void Update(uint32_t deltaTime) override;

	private:
		SDL_Window* m_Window;
		bool m_bIsFramebufferResized = false;
		bool m_bIsWindowMinimized = false;

		struct SWindowData
		{
			const char* Title;
			int Width;
			int Height;
		};
		SWindowData m_Data;
	};

}

#endif
