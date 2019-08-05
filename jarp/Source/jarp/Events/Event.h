#pragma once

namespace jarp {

	enum EventType
	{
		EventTypeNone = 0,

		// Application Events
		EventTypeWindowClosed = 0x01,
		EventTypeWindowMinimized = 0x02,
		EventTypeWindowRestored = 0x03,
		EventTypeWindowResized = 0x04,

		// Key Events
		EventTypeKeyPressed = 0x05,
		EventTypeKeyReleased = 0x06,

		// Mouse Events
		EventTypeMouseButtonPressed = 0x07,
		EventTypeMouseButtonReleased = 0x08,
		EventTypeMouseMoved = 0x09,
	};

	enum EventCategory
	{
		EventCategoryNone = 0,
		EventCategoryApplication	= 1 << 0,
		EventCategoryInput			= 1 << 1,
		EventCategoryKeyboard		= 1 << 2,
		EventCategoryMouse			= 1 << 3,
		EventCategoryMouseButton	= 1 << 4,

		EventCategoryCOUNT = 5
	};

#define EVENT_CLASS_TYPE(Type)	virtual EventType GetEventType() const override { return Type; }\
								virtual const char* GetName() const override { return #Type; }

#define EVENT_CLASS_CATEGORY(Category)	virtual int GetCategoryFlags() const override { return Category; }

	class Event
	{
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;

		inline bool IsInCategory(EventCategory Category) { return Category & GetCategoryFlags(); }

	public:
		bool bIsHandled = false;
	};

}
