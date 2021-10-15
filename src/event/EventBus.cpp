#include "EventBus.h"

EventBus& EventBus::get_instance()
{
    static EventBus instance;
    return instance;
}

void EventBus::subscribe(EventType type, const EventUpdateFunction& function)
{
    if (!this->handlers.contains(type)) {
        this->handlers.emplace(type, std::vector{ function });
    }
    else {
        this->handlers.at(type).push_back(function);
    }
}

void EventBus::unsubscribe(EventType type)
{
}

void EventBus::publish(EventType type)
{
    if (!this->handlers.contains(type)) {
        return;
    }
    const auto& handlers_for_type = this->handlers.at(type);
    for (const auto& handler : handlers_for_type) {
        handler();
    }
}
