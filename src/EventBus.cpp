#include "EventBus.hpp"
#include "Logger.hpp"

#include <algorithm>
#include <utility>

int EventBus::Subscribe(Handler handler) {
    const int id = nextSubscriptionId++;
    handlers.emplace_back(id, std::move(handler));
    return id;
}

void EventBus::Unsubscribe(int subscriptionId) {
    handlers.erase(
        std::remove_if(
            handlers.begin(),
            handlers.end(),
            [subscriptionId](const auto& entry) { return entry.first == subscriptionId; }
        ),
        handlers.end()
    );
}

void EventBus::Publish(const GameEvent& event) const {
    if (!event.payload.IsConsistentFor(event.type)) {
        Logger::Instance().Warn("Event payload is inconsistent for event type: " + event.message);
    }

    for (const auto& entry : handlers) {
        entry.second(event);
    }
}
