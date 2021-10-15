#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <functional>
#include <unordered_map>
#include <vector>
#include "Event.h"

class EventBus {
protected:
    EventBus() = default;
    EventBus(const EventBus&) = default;
    EventBus(EventBus&&) = default;
    EventBus& operator=(const EventBus&) = default;
    EventBus& operator=(EventBus&&) = default;
public:
    ~EventBus() = default;
    static EventBus& get_instance();

    using EventUpdateFunction = std::function<void()>;

    void subscribe(EventType type, const EventUpdateFunction& function);
    void unsubscribe(EventType type);

    void publish(EventType type);
private:
    std::unordered_map<EventType, std::vector<EventUpdateFunction>> handlers;
};

#endif
