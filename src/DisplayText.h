#pragma once

#include "IDisplay.h"

#include <json/json.h>
#include <iostream>

namespace richman {

/// 文本展示实现：将信息输出到标准输出流
class DisplayText : public IDisplay {
public:
    DisplayText() = default;

    void show(const Json::Value& json) override;
};

}  // namespace richman
