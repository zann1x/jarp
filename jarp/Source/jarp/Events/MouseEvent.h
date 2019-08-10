#pragma once

#include "Event.h"

namespace jarp {

	class MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return MouseButton; }

		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse | EventCategoryMouseButton)

	protected:
		MouseButtonEvent(void* NativeEvent, int Button)
			: Event(NativeEvent), MouseButton(Button) { }

	private:
		int MouseButton;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(void* NativeEvent, int Button)
			: MouseButtonEvent(NativeEvent, Button) { }

		EVENT_CLASS_TYPE(EventTypeMouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(void* NativeEvent, int Button)
			: MouseButtonEvent(NativeEvent, Button) { }

		EVENT_CLASS_TYPE(EventTypeMouseButtonReleased)
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(void* NativeEvent, int XPos, int YPos)
			: Event(NativeEvent), MouseX(XPos), MouseY(YPos) { }

		inline int GetXPos() const { return MouseX; }
		inline int GetYPos() const { return MouseY; }

		EVENT_CLASS_TYPE(EventTypeMouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)

	private:
		int MouseX;
		int MouseY;
	};

}
