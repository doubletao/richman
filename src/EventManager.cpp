#include "EventManager.h"

#include <random>

namespace richman {

EventManager::EventManager() : rng_(std::random_device{}()) {}

EventCard EventManager::getCard() {
    if (cards_.empty())
        return EventCard{};

    std::uniform_int_distribution<std::size_t> dist(0, cards_.size() - 1);
    return cards_[dist(rng_)];
}

}  // namespace richman
