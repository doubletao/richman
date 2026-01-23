#pragma once

#include <json/json.h>
#include <string>

namespace richman {

/// 展示接口：用于文本、socket等不同展示方式的统一接口
class IDisplay {
public:
    virtual ~IDisplay() = default;

    /// 展示信息（接受 JSON 格式）
    /// @param json JSON 对象，包含展示内容（如 {"type": "text", "content": "..."}）
    virtual void show(const Json::Value& json) = 0;
};

}  // namespace richman
