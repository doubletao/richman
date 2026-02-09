#include "GameEngine.h"

#include "EventManager.h"

#include <json/json.h>
#include <sstream>

namespace richman {

GameEngine::GameEngine(DataManager& data_manager, DisplayManager* display_manager)
    : data_manager_(data_manager), display_manager_(display_manager), rng_(std::random_device{}()) {}

void GameEngine::start() {
    if (data_manager_.getPlayers().empty()) {
        output("错误：没有玩家，无法开始游戏");
        return;
    }

    output("游戏开始！");
    current_player_index_ = 0;
    game_over_ = false;

    // 游戏主循环：轮到当前玩家，若需输入则通过 input_callback_ 获取并处理，nextPlayer 由内部逻辑在回合结束时调用
    while (!game_over_) {
        processPlayerTurn();
        while (waiting_for_input_ && !game_over_ && input_callback_) {
            Json::Value input = input_callback_();
            processInput(input);
        }
    }
}

void GameEngine::processPlayerTurn() {
    auto& players = data_manager_.getPlayers();
    if (current_player_index_ >= static_cast<int>(players.size())) {
        output("错误：当前玩家索引超出范围，无法进行回合");
        return;
    }

    Player& current_player = players[current_player_index_];

    // 检查玩家是否破产
    if (isPlayerBankrupt(current_player_index_)) {
        output("玩家 " + current_player.getName() + " 已破产，退出游戏");
        checkGameEnd();
        return;
    }

    // 检查是否冻结
    if (current_player.isFrozen()) {
        output("玩家 " + current_player.getName() + " 处于冻结状态，跳过本回合");
        current_player.tickRound();
        return;
    }

    // 回合开始提示
    std::ostringstream oss;
    oss << "玩家 " << current_player.getName() << " 回合开始，输入a摇骰子";
    output(oss.str());

    // 等待输入
    waiting_for_input_ = true;
    pending_action_ = "roll";
}

bool GameEngine::processInput(const Json::Value& input) {
    if (!waiting_for_input_)
        return false;

    // 验证输入格式
    if (!input.isMember("player") || !input.isMember("action")) {
        output("错误：输入格式不正确，需要包含 'player' 和 'action' 字段");
        return false;
    }

    std::string player_name = input["player"].asString();
    std::string action = input["action"].asString();

    auto& players = data_manager_.getPlayers();
    if (current_player_index_ >= static_cast<int>(players.size())) {
        output("错误：当前玩家索引超出范围，无法处理输入");
        return false;
    }

    // 验证是否为当前玩家
    if (players[current_player_index_].getName() != player_name) {
        std::ostringstream oss;
        oss << "错误：当前不是玩家 " << player_name << " 的回合，请等待当前玩家操作";
        output(oss.str());
        return false;
    }

    if (pending_action_ == "roll") {
        if (action == "a") {
            waiting_for_input_ = false;
            int dice = rollDice();
            std::ostringstream oss;
            oss << "玩家 " << player_name << " 摇出 " << dice << " 点";
            output(oss.str());
            movePlayer(current_player_index_, dice);
            return true;
        }
        std::ostringstream oss;
        oss << "错误：摇骰子必须输入 'a'，请重试";
        output(oss.str());
        return false;
    } 
    else if (pending_action_ == "buy") {
        waiting_for_input_ = false;
        if (action == "a" && pending_land_index_ >= 0) {
            handleBuyLand(current_player_index_, pending_land_index_);
        } 
        else if (action == "b" && pending_land_index_ >= 0) {
            // 忽略购买
            output("玩家 " + player_name + " 选择忽略购买");
            nextPlayer();
        }
        else {
            output("错误：购买操作输入格式不正确，请输入 'a' 购买或 'b' 忽略，请重试");
            waiting_for_input_ = true;  // 重新等待输入
            return false;
        }
        pending_land_index_ = -1;
        return true;
    } 
    else if (pending_action_ == "upgrade") 
    {
        waiting_for_input_ = false;
        if (action == "a" && pending_land_index_ >= 0) 
        {
            handleUpgradeLand(current_player_index_, pending_land_index_);
        } 
        else if (action == "b" && pending_land_index_ >= 0)
        {
            // 忽略升级
            output("玩家 " + player_name + " 选择忽略升级");
            nextPlayer();
        }
        else {
            output("错误：升级操作输入格式不正确，请输入 'a' 升级或 'b' 忽略，请重试");
            waiting_for_input_ = true;  // 重新等待输入
            return false;
        }
        pending_land_index_ = -1;
        return true;
    }

    return false;
}

int GameEngine::rollDice() {
    std::uniform_int_distribution<int> dist(1, 6);
    return dist(rng_);
}

void GameEngine::movePlayer(int player_index, int steps) {
    auto& players = data_manager_.getPlayers();
    auto& lands = data_manager_.getLands();
    if (player_index >= static_cast<int>(players.size()) || lands.empty()) {
        std::ostringstream oss;
        oss << "错误：玩家索引 " << player_index << " 超出范围或地图为空，无法移动玩家";
        output(oss.str());
        return;
    }

    Player& player = players[player_index];
    int start_pos = player.getPosition();
    int map_size = static_cast<int>(lands.size());

    // 逐步移动并输出
    for (int i = 1; i <= steps; ++i) 
    {
        int new_pos = (start_pos + i) % map_size;
        const Land& land = lands[new_pos];

        std::ostringstream oss;
        if (new_pos == 0 && i < steps) 
        {
            // 路过起点
            int pass_money = land.getPassMoney();
            player.addMoney(pass_money);
            oss << "玩家 " << player.getName() << " 路过起始点，获得 " << pass_money << " 元";
        }
        else if (i == steps) 
        {
            // 到达目标地块
            oss << "玩家 " << player.getName() << " 到达 " << land.getName();
        } 
        else 
        {
            // 路过其他地块
            oss << "玩家 " << player.getName() << " 路过 " << land.getName();
        }
        output(oss.str());
    }

    // 更新位置
    int final_pos = (start_pos + steps) % map_size;
    player.setPosition(final_pos);

    // 处理到达地块
    handleLandArrival(player_index, final_pos);
}

void GameEngine::handleLandArrival(int player_index, int land_index) {
    auto& players = data_manager_.getPlayers();
    auto& lands = data_manager_.getLands();
    if (player_index >= static_cast<int>(players.size()) || land_index >= static_cast<int>(lands.size())) {
        std::ostringstream oss;
        oss << "错误：玩家索引 " << player_index << " 或地块索引 " << land_index 
            << " 超出范围，无法处理到达地块";
        output(oss.str());
        return;
    }

    Player& player = players[player_index];
    Land& land = lands[land_index];

    switch (land.getType()) {
        case LandType::Start: {
            // 起点：已处理过路过，这里不再处理
            nextPlayer();
            break;
        }
        case LandType::Event: {
            // 事件：抽卡
            EventCard card = data_manager_.getEventManager().getCard();
            std::ostringstream oss;
            oss << "玩家 " << player.getName() << " 触发事件：" << card.description;
            output(oss.str());

            // 应用事件卡效果
            player.addMoney(card.money_delta_self);
            player.setFreezeRounds(std::max(0, player.getFreezeRounds() + card.freeze_delta_self));
            int new_pos = (player.getPosition() + card.position_delta_self) % static_cast<int>(lands.size());
            if (new_pos < 0) new_pos += lands.size();
            player.setPosition(new_pos);

            // 其他玩家
            for (int i = 0; i < static_cast<int>(players.size()); ++i) {
                if (i != player_index) {
                    players[i].addMoney(card.money_delta_others);
                    players[i].setFreezeRounds(std::max(0, players[i].getFreezeRounds() + card.freeze_delta_others));
                    int other_pos = (players[i].getPosition() + card.position_delta_others) % static_cast<int>(lands.size());
                    if (other_pos < 0) other_pos += lands.size();
                    players[i].setPosition(other_pos);
                }
            }

            nextPlayer();
            break;
        }
        case LandType::Basic: {
            if (!land.isOwned()) 
            {
                // 无主：可以购买
                int price = land.getPurchasePrice(1);
                std::ostringstream oss;
                oss << "玩家 " << player.getName() << " 可以购买 " << land.getName()
                    << "，价格 " << price << " 元，输入a购买，输入其他忽略";
                output(oss.str());
                waiting_for_input_ = true;
                pending_action_ = "buy";
                pending_land_index_ = land_index;
            } 
            else if 
            (land.getOwnerIndex() == player_index) 
            {
                // 自己的地：可以升级
                if (land.getLevel() < 4) {
                    int upgrade_price = land.getPurchasePrice(land.getLevel() + 1) - land.getPurchasePrice(land.getLevel());
                    std::ostringstream oss;
                    oss << "玩家 " << player.getName() << " 可以升级 " << land.getName()
                        << " 到 " << (land.getLevel() + 1) << " 级，价格 " << upgrade_price
                        << " 元，输入a升级，输入其他忽略";
                    output(oss.str());
                    waiting_for_input_ = true;
                    pending_action_ = "upgrade";
                    pending_land_index_ = land_index;
                } else {
                    nextPlayer();
                }
            } 
            else 
            {
                // 别人的地：缴纳过路费
                int owner_index = land.getOwnerIndex();
                int toll = land.getToll(land.getLevel());
                player.addMoney(-toll);
                players[owner_index].addMoney(toll);
                std::ostringstream oss;
                oss << "玩家 " << player.getName() << " 向玩家 " << players[owner_index].getName()
                    << " 缴纳过路费 " << toll << " 元";
                output(oss.str());
                checkGameEnd();
                if (!game_over_) {
                    nextPlayer();
                }
            }
            break;
        }
        case LandType::Chain: 
        {
            if (!land.isOwned()) {
                // 无主：可以购买
                int price = land.getSalePrice();
                std::ostringstream oss;
                oss << "玩家 " << player.getName() << " 可以购买 " << land.getName()
                    << "，价格 " << price << " 元，输入a购买，输入其他忽略";
                output(oss.str());
                waiting_for_input_ = true;
                pending_action_ = "buy";
                pending_land_index_ = land_index;
            } 
            else if (land.getOwnerIndex() == player_index) 
            {
                // 自己的地：无需操作
                nextPlayer();
            } 
            else 
            {
                // 别人的地：缴纳过路费（按拥有连锁数量）
                int owner_index = land.getOwnerIndex();
                int chain_count = 0;
                for (const auto& l : lands) {
                    if (l.getType() == LandType::Chain && l.getOwnerIndex() == owner_index) {
                        chain_count++;
                    }
                }
                int toll = land.getChainToll(chain_count);
                player.addMoney(-toll);
                players[owner_index].addMoney(toll);
                std::ostringstream oss;
                oss << "玩家 " << player.getName() << " 向玩家 " << players[owner_index].getName()
                    << " 缴纳过路费 " << toll << " 元";
                output(oss.str());
                checkGameEnd();
                if (!game_over_) {
                    nextPlayer();
                }
            }
            break;
        }
    }
}

void GameEngine::handleBuyLand(int player_index, int land_index) {
    auto& players = data_manager_.getPlayers();
    auto& lands = data_manager_.getLands();
    if (player_index >= static_cast<int>(players.size()) || land_index >= static_cast<int>(lands.size()))
        return;

    Player& player = players[player_index];
    Land& land = lands[land_index];

    if (land.isOwned()) {
        output("该地块已被购买");
        nextPlayer();
        return;
    }

    int price = 0;
    if (land.getType() == LandType::Basic) {
        price = land.getPurchasePrice(1);
    } else if (land.getType() == LandType::Chain) {
        price = land.getSalePrice();
    }

    if (player.getMoney() < price) {
        std::ostringstream oss;
        oss << "玩家 " << player.getName() << " 金钱不足，买地失败";
        output(oss.str());
        nextPlayer();
        return;
    }

    player.addMoney(-price);
    land.setOwnerIndex(player_index);
    land.setLevel(1);
    std::ostringstream oss;
    oss << "玩家 " << player.getName() << " 购买了 " << land.getName();
    output(oss.str());
    nextPlayer();
}

void GameEngine::handleUpgradeLand(int player_index, int land_index) {
    auto& players = data_manager_.getPlayers();
    auto& lands = data_manager_.getLands();
    if (player_index >= static_cast<int>(players.size()) || land_index >= static_cast<int>(lands.size()))
        return;

    Player& player = players[player_index];
    Land& land = lands[land_index];

    if (land.getOwnerIndex() != player_index || land.getLevel() >= 4) {
        output("无法升级该地块");
        nextPlayer();
        return;
    }

    int current_level = land.getLevel();
    int upgrade_price = land.getPurchasePrice(current_level + 1) - land.getPurchasePrice(current_level);

    if (player.getMoney() < upgrade_price) {
        std::ostringstream oss;
        oss << "玩家 " << player.getName() << " 金钱不足，升级失败";
        output(oss.str());
        nextPlayer();
        return;
    }

    player.addMoney(-upgrade_price);
    land.setLevel(current_level + 1);
    std::ostringstream oss;
    oss << "玩家 " << player.getName() << " 将 " << land.getName() << " 升级到 " << land.getLevel() << " 级";
    output(oss.str());
    nextPlayer();
}

bool GameEngine::isPlayerBankrupt(int player_index) const {
    const auto& players = data_manager_.getPlayers();
    if (player_index >= static_cast<int>(players.size()))
        return true;
    return players[player_index].getMoney() < 0;
}

void GameEngine::checkGameEnd() {
    const auto& players = data_manager_.getPlayers();
    int active_count = getActivePlayerCount();

    if (active_count <= 1) {
        game_over_ = true;
        for (int i = 0; i < static_cast<int>(players.size()); ++i) {
            if (!isPlayerBankrupt(i)) {
                std::ostringstream oss;
                oss << "玩家 " << players[i].getName() << " 取得了最终的胜利";
                output(oss.str());
                break;
            }
        }
    }
}

int GameEngine::getActivePlayerCount() const {
    const auto& players = data_manager_.getPlayers();
    int count = 0;
    for (int i = 0; i < static_cast<int>(players.size()); ++i) {
        if (!isPlayerBankrupt(i)) {
            count++;
        }
    }
    return count;
}

void GameEngine::nextPlayer() {
    const auto& players = data_manager_.getPlayers();
    current_player_index_ = (current_player_index_ + 1) % static_cast<int>(players.size());
    waiting_for_input_ = false;
    pending_action_.clear();
    pending_land_index_ = -1;
}

void GameEngine::output(const std::string& text) {
    if (display_manager_) {
        Json::Value json;
        json["type"] = "text";
        json["content"] = text;
        display_manager_->show(json);
    }
}

}  // namespace richman
