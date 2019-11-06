#include "jarppch.h"
#include "EventBus.h"

#include "jarp/Core.h"

namespace jarp {

	EventBus* EventBus::s_Instance = new EventBus();

	EventBus::EventBus()
	{
		JARP_CORE_ASSERT(!s_Instance, "EventBus instance already exists!");
		s_Instance = this;
	}

	void EventBus::Register(EventType type, EventListener* listener)
	{
		m_EventTypeSubscribers[type].push_back(listener);
	}

	void EventBus::Deregister(EventType type, EventListener* listener)
	{
		auto& listeners = m_EventTypeSubscribers[type];
		listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
	}

	void EventBus::Register(EventCategory category, EventListener* listener)
	{
		m_EventCategorySubscribers[category].push_back(listener);
	}

	void EventBus::Deregister(EventCategory category, EventListener* listener)
	{
		auto& listeners = m_EventCategorySubscribers[category];
		listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
	}

	void EventBus::Dispatch(Event& event)
	{
		// Publish the event to all subscribers of its specific type
		{
			auto& typeListeners = m_EventTypeSubscribers[event.GetEventType()];
			for (EventListener* listener : typeListeners)
			{
				listener->OnEvent(event);
				if (event.m_bIsHandled)
					break;
			}
		}

		// Publish the event to all subscribers of the general category
		{
			for (int i = 0; i < EventCategoryCOUNT; ++i)
			{
				int tmp = event.GetCategoryFlags() & (1 << i);
				auto& categoryListeners = m_EventCategorySubscribers[event.GetCategoryFlags() & (1 << i)];
				for (EventListener* listener : categoryListeners)
				{
					listener->OnEvent(event);
					if (event.m_bIsHandled)
						break;
				}
			}
		}
	}

}
