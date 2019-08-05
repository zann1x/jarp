#include "jarppch.h"
#include "EventBus.h"

namespace jarp {

	EventBus* EventBus::Instance = new EventBus();

	EventBus::EventBus()
	{
		JARP_CORE_ASSERT(!Instance, "EventBus instance already exists!");
		Instance = this;
	}

	void EventBus::Register(EventType Type, EventListener* Listener)
	{
		EventTypeSubscribers[Type].push_back(Listener);
	}

	void EventBus::Deregister(EventType Type, EventListener* Listener)
	{
		auto& Listeners = EventTypeSubscribers[Type];
		Listeners.erase(std::remove(Listeners.begin(), Listeners.end(), Listener), Listeners.end());
	}

	void EventBus::Register(EventCategory Category, EventListener* Listener)
	{
		EventCategorySubscribers[Category].push_back(Listener);
	}

	void EventBus::Deregister(EventCategory Category, EventListener* Listener)
	{
		auto& Listeners = EventCategorySubscribers[Category];
		Listeners.erase(std::remove(Listeners.begin(), Listeners.end(), Listener), Listeners.end());
	}

	void EventBus::Dispatch(Event& E)
	{
		// Publish the event to all subscribers of its specific type
		{
			auto& TypeListeners = EventTypeSubscribers[E.GetEventType()];
			for (EventListener* Listener : TypeListeners)
			{
				Listener->OnEvent(E);
				if (E.bIsHandled)
					break;
			}
		}

		// Publish the event to all subscribers of the general category
		{
			for (int i = 0; i < EventCategoryCOUNT; ++i)
			{
				int tmp = E.GetCategoryFlags() & (1 << i);
				auto& CategoryListeners = EventCategorySubscribers[E.GetCategoryFlags() & (1 << i)];
				for (EventListener* Listener : CategoryListeners)
				{
					Listener->OnEvent(E);
					if (E.bIsHandled)
						break;
				}
			}
		}
	}

}
