#include "DataManager.h"
#include <iostream>

int main() {
    richman::DataManager dm;
    if (dm.loadMap("config/china.json")) {
        std::cout << "地图: " << dm.getMapName() << ", 地块数: " << dm.getLands().size() << std::endl;
    } else {
        std::cout << "加载地图失败" << std::endl;
    }
    return 0;
}
