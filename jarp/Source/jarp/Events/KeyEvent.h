#pragma once

#include "Event.h"

namespace jarp {

	class KeyEvent : public Event
	{
	public:
		inline int GetKeycode() const { return m_Keycode; }

		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

	protected:
		KeyEvent(int keycode)
			: m_Keycode(keycode) { }

	private:
		int m_Keycode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode)
			: KeyEvent(keycode) { }

		EVENT_CLASS_TYPE(EventTypeKeyPressed)
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) { }

		EVENT_CLASS_TYPE(EventTypeKeyReleased)
	};

}
