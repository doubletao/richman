#pragma once

#include "IDisplay.h"

#include <json/json.h>
#include <vector>
#include <memory>

namespace richman {

/// 展示管理器：统一管理多个展示实现，同步输出到所有 Display
class DisplayManager : public IDisplay {
public:
    DisplayManager() = default;

    /// 添加展示实现（DisplayManager 不拥有所有权，由调用者管理生命周期）
    void addDisplay(IDisplay* display);

    /// 添加展示实现（DisplayManager 拥有所有权，使用智能指针管理）
    void addDisplay(std::unique_ptr<IDisplay> display);

    /// 移除展示实现（仅移除指针，不删除对象）
    void removeDisplay(IDisplay* display);

    /// 清空所有展示实现
    void clear();

    /// 获取展示实现数量
    size_t getDisplayCount() const { return displays_.size(); }

    /// 实现 IDisplay 接口：向所有注册的 Display 同步输出
    void show(const Json::Value& json) override;

private:
    std::vector<IDisplay*> displays_;
    std::vector<std::unique_ptr<IDisplay>> owned_displays_;  // 拥有所有权的 Display
};

}  // namespace richman
