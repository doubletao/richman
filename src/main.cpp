#include "DataManager.h"
#include "DisplayManager.h"
#include "DisplayText.h"
#include "GameEngine.h"

#include <json/json.h>
#include <cctype>
#include <iostream>
#include <memory>
#include <string>

#ifdef __linux__
#include <unistd.h>
#include <limits.h>
#endif

/// 获取可执行文件所在目录（末尾带 /），便于解析 config 等相对路径
static std::string getExeDir() {
#ifdef __linux__
    char buf[PATH_MAX];
    ssize_t n = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (n <= 0) return "";
    buf[n] = '\0';
    std::string path(buf);
    std::string::size_type pos = path.find_last_of('/');
    if (pos == std::string::npos) return "";
    return path.substr(0, pos + 1);
#else
    return "";
#endif
}

int main() {
    richman::DataManager dm;
    std::string binDir = getExeDir();  // 运行目录为 bin/ 时，配置文件相对 bin/

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

    std::string configPath = binDir + "config.json";
    std::string mapFullPath = binDir + mapPath;
    if (!dm.loadAll(configPath, mapFullPath)) {
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
            // 去除首尾空格，支持 show 等完整命令
            auto trim = [](std::string& s) {
                auto start = s.find_first_not_of(" \t\r\n");
                if (start == std::string::npos) { s.clear(); return; }
                auto end = s.find_last_not_of(" \t\r\n");
                s = s.substr(start, end == std::string::npos ? std::string::npos : end - start + 1);
            };
            trim(action);
            if (!action.empty()) {
                if (action.size() == 4 && std::tolower(static_cast<unsigned char>(action[0])) == 's'
                    && std::tolower(static_cast<unsigned char>(action[1])) == 'h'
                    && std::tolower(static_cast<unsigned char>(action[2])) == 'o'
                    && std::tolower(static_cast<unsigned char>(action[3])) == 'w')
                    input["action"] = "show";
                else
                    input["action"] = action;  // 整行传入，支持聊天完整内容及 s/y/n 单键
            } else
                input["action"] = "";
        } else
            input["action"] = "";
        return input;
    });

    engine.start();
    std::cout << "游戏结束。" << std::endl;
    return 0;
}
