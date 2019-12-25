#pragma once

#include <memory>
#include <utility>

namespace jarp {

	struct WindowProperties
	{
		const char* Title;
		int Width;
		int Height;

		WindowProperties(const char* title = "jarp", int width = 1280, int height = 720)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	class Window
	{
	public:
		virtual ~Window() = default;

		virtual void Update(uint32_t deltaTime) = 0;

		virtual std::pair<int, int> GetFramebufferSize() const = 0;
		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
		virtual const char* GetTitle() const = 0;
		virtual void* GetNativeWindow() const = 0;

		virtual bool IsMinimized() const = 0;
		virtual bool IsFramebufferResized() const = 0;
		virtual void SetFramebufferResized(bool bIsFramebufferResized) = 0;

		static Window* Create(const WindowProperties& properties = WindowProperties());
	};

}
