#pragma once

#include <array>
#include <string>

namespace richman {

/// 地块类型
enum class LandType {
    Basic,   ///< 基本：可购买、升级，对手支付过路费
    Chain,   ///< 连锁：仅售价，过路费按拥有连锁数量
    Event,   ///< 事件：触发抽卡（卡片在事件管理器中）
    Start    ///< 起点：路过发钱，玩家起始位置
};

/// 地块：地图上的一个格子，支持四种基础类型
class Land {
public:
    // ---------- 基本 (Basic) ----------
    /// 构造基本地块
    /// @param name 地块名
    /// @param purchase_prices 等级 1~4 的购买/升级价位（0: 购买到1级, 1: 1->2, 2: 2->3, 3: 3->4）
    /// @param tolls 等级 1~4 对应的过路费
    Land(LandType type, std::string name,
         const std::array<int, 4>& purchase_prices,
         const std::array<int, 4>& tolls);

    // ---------- 连锁 (Chain) ----------
    /// 构造连锁地块
    /// @param name 地块名
    /// @param sale_price 售价（不可升级）
    /// @param chain_tolls 拥有 1 / 2 / 3 / 4+ 个连锁时的过路费
    Land(LandType type, std::string name,
         int sale_price,
         const std::array<int, 4>& chain_tolls);

    // ---------- 事件 (Event) ----------
    /// 构造事件地块（仅类型与名称，抽卡逻辑在事件管理器中）
    Land(LandType type, std::string name);

    // ---------- 起点 (Start) ----------
    /// 构造起点
    /// @param name 地块名
    /// @param pass_money 每次路过起点发放的金钱
    Land(LandType type, std::string name, int pass_money);

    LandType getType() const { return type_; }
    const std::string& getName() const { return name_; }

    // --- 基本 (Basic) 专用，level 为 1~4 ---
    int getPurchasePrice(int level) const;
    int getToll(int level) const;

    // --- 连锁 (Chain) 专用 ---
    int getSalePrice() const { return sale_price_; }
    /// 按拥有连锁数量计算过路费，超过 4 个按 4 个计
    int getChainToll(int chain_count) const;

    // --- 起点 (Start) 专用 ---
    int getPassMoney() const { return pass_money_; }

private:
    LandType type_;
    std::string name_;

    // 基本：等级 1~4 的购买价位与过路费
    std::array<int, 4> purchase_prices_{};
    std::array<int, 4> tolls_{};

    // 连锁：售价与 1/2/3/4+ 连锁的过路费
    int sale_price_ = 0;
    std::array<int, 4> chain_tolls_{};

    // 起点：路过发放金额
    int pass_money_ = 0;
};

}  // namespace richman
