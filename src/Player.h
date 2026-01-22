#pragma once

#include <string>

namespace richman {

/// 玩家
class Player {
public:
    /// @param initial_money 初始金钱
    /// @param name 名称，DataManager::addPlayer 默认为 player_1, player_2, ...
    Player(int initial_money, std::string name);

    // ---------- 名称 ----------
    const std::string& getName() const { return name_; }
    void setName(std::string s) { name_ = std::move(s); }

    // ---------- 金钱 ----------
    int getMoney() const { return money_; }
    void setMoney(int m) { money_ = m; }
    void addMoney(int delta) { money_ += delta; }

    // ---------- 当前坐标（地图 vector 的 index） ----------
    int getPosition() const { return position_; }
    void setPosition(int index) { position_ = index; }

    // ---------- 冻结状态 ----------
    /// 冻结剩余回合数：0 = 正常，>0 = 冻结（不可摇骰子、不可收过路费）
    int getFreezeRounds() const { return freeze_rounds_; }
    void setFreezeRounds(int n) { freeze_rounds_ = n; }

    /// 是否处于冻结态
    bool isFrozen() const { return freeze_rounds_ > 0; }

    /// 每回合调用：若处于冻结态则减一，到 0 恢复正常
    void tickRound();

private:
    std::string name_;
    int money_ = 0;
    int position_ = 0;
    int freeze_rounds_ = 0;  // 0=正常, >0=冻结剩余回合
};

}  // namespace richman
