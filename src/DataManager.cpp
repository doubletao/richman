#include "DataManager.h"

#include <json/json.h>

#include <fstream>
#include <string>

namespace richman {

bool DataManager::loadMap(const std::string& path) {
    std::ifstream f(path);
    if (!f)
        return false;

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(f, root))
        return false;

    if (!root.isMember("lands") || !root["lands"].isArray())
        return false;

    map_name_ = root.get("name", Json::Value("")).asString();
    map_id_ = root.get("id", Json::Value("")).asString();

    const Json::Value& arr = root["lands"];
    std::vector<Land> temp;
    temp.reserve(arr.size());

    for (Json::ArrayIndex i = 0; i < arr.size(); ++i) {
        const Json::Value& el = arr[i];
        std::string type = el.get("type", Json::Value("")).asString();
        std::string name = el.get("name", Json::Value("")).asString();

        if (type == "basic") {
            if (!el.isMember("purchase_prices") || !el.isMember("tolls"))
                continue;
            std::array<int, 4> pp{}, tt{};
            for (int j = 0; j < 4; ++j)
                pp[j] = el["purchase_prices"][j].asInt();
            for (int j = 0; j < 4; ++j)
                tt[j] = el["tolls"][j].asInt();
            temp.emplace_back(LandType::Basic, name, pp, tt);
        } else if (type == "chain") {
            if (!el.isMember("sale_price") || !el.isMember("chain_tolls"))
                continue;
            int sp = el["sale_price"].asInt();
            std::array<int, 4> ct{};
            for (int j = 0; j < 4; ++j)
                ct[j] = el["chain_tolls"][j].asInt();
            temp.emplace_back(LandType::Chain, name, sp, ct);
        } else if (type == "event") {
            temp.emplace_back(LandType::Event, name);
        } else if (type == "start") {
            if (!el.isMember("pass_money"))
                continue;
            int pm = el["pass_money"].asInt();
            temp.emplace_back(LandType::Start, name, pm);
        }
    }

    lands_ = std::move(temp);
    return true;
}

void DataManager::addPlayer() {
    int n = static_cast<int>(players_.size()) + 1;
    players_.emplace_back(starting_money_, "player_" + std::to_string(n));
}

bool DataManager::loadEventCards(const std::string& path) {
    std::ifstream f(path);
    if (!f)
        return false;

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(f, root))
        return false;

    if (!root.isMember("event_cards") || !root["event_cards"].isArray())
        return false;

    const Json::Value& arr = root["event_cards"];
    for (Json::ArrayIndex i = 0; i < arr.size(); ++i) {
        const Json::Value& el = arr[i];
        EventCard card;
        card.description = el.get("description", Json::Value("")).asString();
        card.money_delta_self = el.get("money_delta_self", Json::Value(0)).asInt();
        card.money_delta_others = el.get("money_delta_others", Json::Value(0)).asInt();
        card.freeze_delta_self = el.get("freeze_delta_self", Json::Value(0)).asInt();
        card.freeze_delta_others = el.get("freeze_delta_others", Json::Value(0)).asInt();
        card.position_delta_self = el.get("position_delta_self", Json::Value(0)).asInt();
        card.position_delta_others = el.get("position_delta_others", Json::Value(0)).asInt();
        event_manager_.addCard(card);
    }

    return true;
}

bool DataManager::loadConfig(const std::string& path) {
    std::ifstream f(path);
    if (!f)
        return false;

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(f, root))
        return false;

    if (root.isMember("game")) {
        const Json::Value& game = root["game"];
        starting_money_ = game.get("starting_money", Json::Value(10000)).asInt();
    }

    return true;
}

bool DataManager::loadAll(const std::string& config_path, const std::string& map_path) {
    // 1. 加载配置（获取初始金钱、默认地图等）
    if (!loadConfig(config_path))
        return false;

    // 2. 加载事件卡
    if (!loadEventCards(config_path))
        return false;

    // 3. 加载地图
    std::string actual_map_path = map_path;
    if (actual_map_path.empty()) {
        // 从配置中读取默认地图
        std::ifstream f(config_path);
        if (f) {
            Json::Value root;
            Json::Reader reader;
            if (reader.parse(f, root) && root.isMember("game")) {
                std::string default_map = root["game"].get("default_map", Json::Value("china")).asString();
                actual_map_path = "config/" + default_map + ".json";
            }
        }
        if (actual_map_path.empty())
            actual_map_path = "config/china.json";  // 兜底
    }

    if (!loadMap(actual_map_path))
        return false;

    return true;
}

}  // namespace richman
