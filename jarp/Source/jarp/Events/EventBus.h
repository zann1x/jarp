#pragma once

#include "jarp/Events/Event.h"
#include "jarp/Events/EventListener.h"

namespace jarp {

	class EventBus
	{
	public:
		EventBus();

		static inline EventBus& Get() { return *s_Instance; }

		// Subscribers of specific event types
		void Register(EventType type, EventListener* listener);
		void Deregister(EventType type, EventListener* listener);

		// Subscribers of whole event categories
		void Register(EventCategory type, EventListener* listener);
		void Deregister(EventCategory type, EventListener* listener);

		void Dispatch(Event& event);

	private:
		static EventBus* s_Instance;

		std::unordered_map<uint32_t, std::vector<EventListener*>> m_EventTypeSubscribers;
		std::unordered_map<uint32_t, std::vector<EventListener*>> m_EventCategorySubscribers;
	};

}
