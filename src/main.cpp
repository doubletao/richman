#include "DataManager.h"
#include "DisplayManager.h"
#include "DisplayText.h"
#include "GameEngine.h"

#include <json/json.h>
#include <iostream>
#include <memory>
#include <string>

int main() {
    richman::DataManager dm;

    // 1. 选择地图
    std::cout << "请选择地图：1=中国 2=美国 3=英国 [1-3]: ";
    int mapChoice = 1;
    std::string mapPath;
    while (true) {
        std::string line;
        if (!std::getline(std::cin, line)) return 1;
        if (line.empty()) break;
        try {
            mapChoice = std::stoi(line);
            if (mapChoice >= 1 && mapChoice <= 3) break;
        } catch (...) {}
        std::cout << "请输入 1、2 或 3，请重试: ";
    }
    if (mapChoice == 1) mapPath = "config/china.json";
    else if (mapChoice == 2) mapPath = "config/american.json";
    else mapPath = "config/england.json";

    if (!dm.loadAll("bin/config.json", mapPath)) {
        std::cout << "加载配置或地图失败" << std::endl;
        return 1;
    }
    std::cout << "已加载地图: " << dm.getMapName() << "，地块数: " << dm.getLands().size() << std::endl;

    // 2. 玩家数量
    int minP = 2, maxP = 4;
    std::cout << "请输入玩家数量 [" << minP << "-" << maxP << "]: ";
    int numPlayers = 2;
    while (true) {
        std::string line;
        if (!std::getline(std::cin, line)) return 1;
        if (line.empty()) break;
        try {
            numPlayers = std::stoi(line);
            if (numPlayers >= minP && numPlayers <= maxP) break;
        } catch (...) {}
        std::cout << "请输入 " << minP << " 到 " << maxP << " 之间的数字，请重试: ";
    }

    // 3. 依次询问玩家姓名
    for (int i = 0; i < numPlayers; ++i) {
        std::cout << "玩家 " << (i + 1) << " 姓名: ";
        std::string name;
        if (!std::getline(std::cin, name)) return 1;
        while (name.empty()) {
            std::cout << "姓名不能为空，请重新输入: ";
            if (!std::getline(std::cin, name)) return 1;
        }
        dm.addPlayer();
        dm.getPlayers().back().setName(name);
    }

    // 4. 启动引擎与展示
    richman::DisplayManager displayMgr;
    displayMgr.addDisplay(std::make_unique<richman::DisplayText>());
    richman::GameEngine engine(dm, &displayMgr);

    engine.setInputCallback([&engine, &dm]() {
        Json::Value input;
        const auto& players = dm.getPlayers();
        int idx = engine.getCurrentPlayerIndex();
        if (idx >= 0 && idx < static_cast<int>(players.size()))
            input["player"] = players[idx].getName();
        else
            input["player"] = "";
        std::string action;
        if (std::getline(std::cin, action)) {
            if (!action.empty())
                input["action"] = action.substr(0, 1);
            else
                input["action"] = "";
        } else
            input["action"] = "";
        return input;
    });

    engine.start();
    std::cout << "游戏结束。" << std::endl;
    return 0;
}
