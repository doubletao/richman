#pragma once

#include <random>
#include <string>
#include <vector>

namespace richman {

/// 事件卡：描述 + 对当前玩家/其他玩家的动作变量（整数，可正可负）
struct EventCard {
    std::string description = "无事发生";

    int money_delta_self = 0;    ///< 当前玩家金钱变化
    int money_delta_others = 0;  ///< 其他玩家金钱变化
    int freeze_delta_self = 0;   ///< 当前玩家冻结变化（加在 freeze_rounds 上）
    int freeze_delta_others = 0; ///< 其他玩家冻结变化
    int position_delta_self = 0; ///< 当前玩家坐标变化
    int position_delta_others = 0; ///< 其他玩家坐标变化
};

/// 事件管理器：持有一副事件卡，支持随机抽卡
class EventManager {
public:
    EventManager();

    const std::vector<EventCard>& getCards() const { return cards_; }
    std::vector<EventCard>& getCards() { return cards_; }

    void addCard(EventCard c) { cards_.push_back(std::move(c)); }

    /// 随机抽取一张卡；若牌堆为空则返回“无事发生”的空白卡
    EventCard getCard();

private:
    std::vector<EventCard> cards_;
    std::mt19937 rng_;
};

}  // namespace richman
