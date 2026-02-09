#include "DisplayText.h"

#include <iostream>

namespace richman {

void DisplayText::show(const Json::Value& json) {
    if (json.isMember("type") && json["type"].asString() == "text" && json.isMember("content")) {
        std::cout << json["content"].asString() << std::endl;
    } else {
        // 如果不是标准格式，输出整个 JSON
        std::cout << json << std::endl;
    }
}

}  // namespace richman
