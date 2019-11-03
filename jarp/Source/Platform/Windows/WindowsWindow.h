#pragma once

#include <volk.h>

#include "SDL.h"
#include "SDL_vulkan.h"

#include "jarp/Window.h"

namespace jarp {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties& Properties = WindowProperties());
		virtual ~WindowsWindow() override;

		inline SDL_Window* GetHandle() { return pWindow; }

		// Windows specific
		HINSTANCE GetNativeInstanceHandle() const;
		HWND GetNativeWindowHandle() const;
		//

		VkResult CreateSurface(const VkInstance Instance, VkSurfaceKHR* SurfaceKHR) const;
		std::vector<const char*> GetInstanceExtensions() const;

		// Returns the framebuffer width as the first parameter and height as the second
		virtual std::pair<int, int> GetFramebufferSize() const override;
		virtual inline int GetWidth() const override { return Data.Width; }
		virtual inline int GetHeight() const override { return Data.Height; }
		virtual inline void* GetNativeWindow() const override { return pWindow; };

		virtual inline bool IsMinimized() const override { return bIsWindowMinimized; }
		inline bool IsFramebufferResized() { return bIsFramebufferResized; }
		void SetFramebufferResized(bool FramebufferResized) { bIsFramebufferResized = FramebufferResized; }

		virtual void Update(uint32_t DeltaTime) override;

	private:
		SDL_Window* pWindow;
		bool bIsFramebufferResized = false;
		bool bIsWindowMinimized = false;

		struct WindowData
		{
			std::string Title;
			int Width;
			int Height;
		} Data;
	};

}
