#include "DisplayManager.h"

#include <algorithm>

namespace richman {

void DisplayManager::addDisplay(IDisplay* display) {
    if (display != nullptr) {
        displays_.push_back(display);
    }
}

void DisplayManager::addDisplay(std::unique_ptr<IDisplay> display) {
    if (display) {
        IDisplay* raw_ptr = display.get();
        owned_displays_.push_back(std::move(display));
        displays_.push_back(raw_ptr);
    }
}

void DisplayManager::removeDisplay(IDisplay* display) {
    auto it = std::remove(displays_.begin(), displays_.end(), display);
    displays_.erase(it, displays_.end());
}

void DisplayManager::clear() {
    displays_.clear();
    owned_displays_.clear();
}

void DisplayManager::show(const Json::Value& json) {
    // 向所有注册的 Display 同步输出
    for (IDisplay* display : displays_) {
        if (display != nullptr) {
            display->show(json);
        }
    }
}

}  // namespace richman
