#pragma once

#include "jarppch.h"
#include "jarp/Events/Event.h"

namespace jarp {

	struct WindowProperties
	{
		std::string Title;
		int Width;
		int Height;

		WindowProperties(const std::string& Title = "jarp", int Width = 1280, int Height = 720)
			: Title(Title), Width(Width), Height(Height) { }
	};

	class Window
	{
	public:
		virtual ~Window() { };

		virtual void Update(uint32_t DeltaTime) = 0;

		virtual std::pair<int, int> GetFramebufferSize() const = 0;
		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
		virtual void* GetNativeWindow() const = 0;

		virtual bool IsMinimized() const = 0;

		static Window* Create(const WindowProperties& Properties = WindowProperties());
	};

}
