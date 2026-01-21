#pragma once

#include "Land.h"

#include <string>
#include <vector>

namespace richman {

/// 数据管理：集中管理游戏中所有配置与静态数据（如地图）
class DataManager {
public:
    /// 从 JSON 文件加载地图，解析 \c lands 数组并填充地图
    /// @param path 地图 JSON 路径（如 config/china.json）
    /// @return 是否加载成功；失败时地图保持不变
    bool loadMap(const std::string& path);

    /// 地图中的地块列表，顺序即棋盘顺序
    const std::vector<Land>& getLands() const { return lands_; }
    std::vector<Land>& getLands() { return lands_; }

    /// 地图名称（来自 JSON 的 \c name ）
    const std::string& getMapName() const { return map_name_; }
    /// 地图 ID（来自 JSON 的 \c id ）
    const std::string& getMapId() const { return map_id_; }

private:
    std::vector<Land> lands_;
    std::string map_name_;
    std::string map_id_;
};

}  // namespace richman
