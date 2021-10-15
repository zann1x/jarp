#ifndef EVENT_H
#define EVENT_H

enum class EventType {
    WINDOW_CLOSE_EVENT
};

class Event {
public:
    virtual ~Event() = default;
    EventType get_event_type() const {
        return this->type;
    }

private:
    EventType type;
};

#endif
