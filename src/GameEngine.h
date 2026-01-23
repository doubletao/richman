#pragma once

#include "DataManager.h"
#include "IDisplay.h"

#include <json/json.h>
#include <random>
#include <string>

namespace richman {

/// 游戏引擎：控制游戏流程、回合循环、玩家操作
class GameEngine {
public:
    /// @param data_manager 数据管理器引用
    /// @param display 展示接口指针（可为 nullptr，但需要时必须有）
    GameEngine(DataManager& data_manager, IDisplay* display = nullptr);

    /// 设置展示接口
    void setDisplay(IDisplay* display) { display_ = display; }

    /// 开始游戏循环
    void start();

    /// 处理输入（JSON 格式：{"player": "player_1", "action": "a"}）
    /// @return 是否处理成功
    bool processInput(const Json::Value& input);

    /// 检查游戏是否结束
    bool isGameOver() const { return game_over_; }

    /// 获取当前玩家索引
    int getCurrentPlayerIndex() const { return current_player_index_; }

private:
    /// 输出文本（通过 IDisplay）
    void output(const std::string& text);

    /// 进入下一个玩家回合
    void nextPlayer();

    /// 处理玩家回合
    void processPlayerTurn();

    /// 摇骰子（返回 1-6）
    int rollDice();

    /// 移动玩家位置
    /// @param steps 移动步数
    void movePlayer(int player_index, int steps);

    /// 处理到达地块的逻辑
    void handleLandArrival(int player_index, int land_index);

    /// 处理购买地块
    void handleBuyLand(int player_index, int land_index);

    /// 处理升级地块
    void handleUpgradeLand(int player_index, int land_index);

    /// 检查玩家是否破产
    bool isPlayerBankrupt(int player_index) const;

    /// 检查游戏结束条件（破产、胜利）
    void checkGameEnd();

    /// 获取活跃玩家数量
    int getActivePlayerCount() const;

    DataManager& data_manager_;
    IDisplay* display_;
    int current_player_index_ = 0;
    bool game_over_ = false;
    bool waiting_for_input_ = false;
    std::string pending_action_;  // 当前等待的操作类型（"roll", "buy", "upgrade"）
    int pending_land_index_ = -1;  // 当前等待操作的地块索引
    std::mt19937 rng_;
};

}  // namespace richman
