#pragma once

#include "EventManager.h"
#include "Land.h"
#include "Player.h"

#include <string>
#include <vector>

namespace richman {

/// 数据管理：集中管理游戏中所有配置与静态数据（如地图、玩家、事件卡）
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

    // ---------- 玩家 ----------
    const std::vector<Player>& getPlayers() const { return players_; }
    std::vector<Player>& getPlayers() { return players_; }

    /// 新增玩家（使用配置中的初始金钱、位置 0、正常态）
    void addPlayer();

    // ---------- 事件管理器 ----------
    EventManager& getEventManager() { return event_manager_; }
    const EventManager& getEventManager() const { return event_manager_; }

    /// 从 JSON 文件加载事件卡到 EventManager
    /// @param path 配置文件路径（如 bin/config.json）
    /// @return 是否加载成功
    bool loadEventCards(const std::string& path);

    // ---------- 配置加载 ----------
    /// 从 JSON 文件加载游戏配置（初始金钱等）
    /// @param path 配置文件路径（如 bin/config.json）
    /// @return 是否加载成功
    bool loadConfig(const std::string& path);

    /// 一次性加载所有配置（地图、事件卡、游戏配置）
    /// @param config_path 配置文件路径（如 bin/config.json）
    /// @param map_path 地图文件路径（如 config/china.json），若为空则使用配置中的 default_map
    /// @return 是否全部加载成功
    bool loadAll(const std::string& config_path, const std::string& map_path = "");

    /// 获取初始金钱（从配置中读取）
    int getStartingMoney() const { return starting_money_; }

private:
    std::vector<Land> lands_;
    std::string map_name_;
    std::string map_id_;
    std::vector<Player> players_;
    EventManager event_manager_;
    int starting_money_ = 10000;  // 默认初始金钱
};

}  // namespace richman
