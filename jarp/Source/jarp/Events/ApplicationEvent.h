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
		WindowResizedEvent(int width, int height)
			: m_Width(width), m_Height(height) { }

		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }

		EVENT_CLASS_TYPE(EventTypeWindowResized)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		int m_Width;
		int m_Height;
	};

}
