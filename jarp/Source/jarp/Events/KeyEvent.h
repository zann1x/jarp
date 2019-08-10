#pragma once

#include "Event.h"

namespace jarp {

	class KeyEvent : public Event
	{
	public:
		inline int GetKeycode() const { return Keycode; }

		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

	protected:
		KeyEvent(void* NativeEvent, int Keycode)
			: Event(NativeEvent), Keycode(Keycode) { }

	private:
		int Keycode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(void* NativeEvent, int Keycode)
			: KeyEvent(NativeEvent, Keycode) { }

		EVENT_CLASS_TYPE(EventTypeKeyPressed)
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(void* NativeEvent, int Keycode)
			: KeyEvent(NativeEvent, Keycode) { }

		EVENT_CLASS_TYPE(EventTypeKeyReleased)
	};

}
