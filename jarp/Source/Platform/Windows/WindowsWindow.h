#pragma once

#include <vulkan/vulkan.h>

#include "SDL.h"
#include "SDL_vulkan.h"

#include "jarp/Input/InputHandler.h"

namespace jarp {

	struct WindowProperties
	{
		std::string Title;
		int Width;
		int Height;

		WindowProperties(const std::string& Title = "jarp", int Width = 1280, int Height = 720)
			: Title(Title), Width(Width), Height(Height) { }
	};

	class WindowsWindow
	{
	public:
		WindowsWindow(const WindowProperties& Properties = WindowProperties());
		~WindowsWindow();

		void Create();
		void Shutdown();

		inline SDL_Window* GetHandle() { return pWindow; }

		HINSTANCE GetNativeInstanceHandle() const;
		HWND GetNativeWindowHandle() const;

		VkResult CreateSurface(const VkInstance Instance, VkSurfaceKHR* SurfaceKHR) const;
		std::vector<const char*> GetInstanceExtensions() const;

		// Returns the framebuffer width as the first parameter and height as the second
		std::pair<int, int> GetFramebufferSize();
		inline int GetWidth() { return Data.Width; }
		inline int GetHeight() { return Data.Height; }

		inline bool IsIconified() { return bIsWindowMinimized; }
		inline bool IsFramebufferResized() { return bIsFramebufferResized; }
		void SetFramebufferResized(bool FramebufferResized) { bIsFramebufferResized = FramebufferResized; }

		bool ShouldClose();
		void Update(uint32_t DeltaTime);

	private:
		SDL_Window* pWindow;
		SDL_Renderer* pRenderer;
		bool bIsFramebufferResized;
		bool bIsWindowMinimized;
		bool bShouldClose;

		InputHandler InputHandler;

		struct WindowData
		{
			std::string Title;
			int Width;
			int Height;
		} Data;
	};

}
