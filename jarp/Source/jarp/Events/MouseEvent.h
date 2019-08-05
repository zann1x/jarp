#pragma once

#include "Event.h"

namespace jarp {

	class MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return MouseButton; }

		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse | EventCategoryMouseButton)

	protected:
		MouseButtonEvent(int Button)
			: MouseButton(Button) { }

	private:
		int MouseButton;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int Button)
			: MouseButtonEvent(Button) { }

		EVENT_CLASS_TYPE(EventTypeMouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int Button)
			: MouseButtonEvent(Button) { }

		EVENT_CLASS_TYPE(EventTypeMouseButtonReleased)
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(int XPos, int YPos)
			: MouseX(XPos), MouseY(YPos) { }

		inline int GetXPos() const { return MouseX; }
		inline int GetYPos() const { return MouseY; }

		EVENT_CLASS_TYPE(EventTypeMouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)

	private:
		int MouseX;
		int MouseY;
	};

}
