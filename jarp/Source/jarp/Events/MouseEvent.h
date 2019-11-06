#pragma once

#include "Event.h"

namespace jarp {

	class MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_MouseButton; }

		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse | EventCategoryMouseButton)

	protected:
		MouseButtonEvent(int button)
			: m_MouseButton(button) { }

	private:
		int m_MouseButton;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button) { }

		EVENT_CLASS_TYPE(EventTypeMouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) { }

		EVENT_CLASS_TYPE(EventTypeMouseButtonReleased)
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(int x, int y)
			: m_MouseX(x), m_MouseY(y) { }

		inline int GetXPos() const { return m_MouseX; }
		inline int GetYPos() const { return m_MouseY; }

		EVENT_CLASS_TYPE(EventTypeMouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)

	private:
		int m_MouseX;
		int m_MouseY;
	};

}
