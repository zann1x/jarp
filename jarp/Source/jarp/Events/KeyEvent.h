#pragma once

#include "Event.h"

namespace jarp {

	class KeyEvent : public Event
	{
	public:
		inline int GetKeycode() const { return Keycode; }

		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

	protected:
		KeyEvent(int Keycode)
			: Keycode(Keycode) { }

	private:
		int Keycode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int Keycode)
			: KeyEvent(Keycode) { }

		EVENT_CLASS_TYPE(EventTypeKeyPressed)
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int Keycode)
			: KeyEvent(Keycode) { }

		EVENT_CLASS_TYPE(EventTypeKeyReleased)
	};

}
