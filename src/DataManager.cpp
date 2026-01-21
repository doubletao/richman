#include "DataManager.h"

#include <json/json.h>

#include <fstream>

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

}  // namespace richman
