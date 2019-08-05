#pragma once

#include "Event.h"

namespace jarp {

	class WindowClosedEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(EventTypeWindowClosed)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowMinimizedEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(EventTypeWindowMinimized)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowRestoredEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(EventTypeWindowRestored)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowResizedEvent : public Event
	{
	public:
		WindowResizedEvent(int Width, int Height)
			: Width(Width), Height(Height) { }

		inline int GetWidth() const { return Width; }
		inline int GetHeight() const { return Height; }

		EVENT_CLASS_TYPE(EventTypeWindowResized)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		int Width;
		int Height;
	};

}
