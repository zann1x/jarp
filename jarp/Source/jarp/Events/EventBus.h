#pragma once

#include "Event.h"
#include "EventListener.h"

#include "jarp/Core.h"

namespace jarp {

	class EventBus
	{
	public:
		EventBus();

		static inline EventBus& Get() { return *Instance; }

		// Subscribers of specific event types
		void Register(EventType Type, EventListener* Listener);
		void Deregister(EventType Type, EventListener* Listener);

		// Subscribers of whole event categories
		void Register(EventCategory Type, EventListener* Listener);
		void Deregister(EventCategory Type, EventListener* Listener);

		void Dispatch(Event& E);

	private:
		static EventBus* Instance;

		std::unordered_map<uint32_t, std::vector<EventListener*>> EventTypeSubscribers;
		std::unordered_map<uint32_t, std::vector<EventListener*>> EventCategorySubscribers;
	};

}
