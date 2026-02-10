#include "Land.h"

namespace richman {

Land::Land(LandType type, std::string name,
           const std::array<int, 4>& purchase_prices,
           const std::array<int, 4>& tolls)
    : type_(type),
      name_(std::move(name)),
      purchase_prices_(purchase_prices),
      tolls_(tolls) {}

Land::Land(LandType type, std::string name,
           int sale_price,
           const std::array<int, 4>& chain_tolls)
    : type_(type),
      name_(std::move(name)),
      sale_price_(sale_price),
      chain_tolls_(chain_tolls) {}

Land::Land(LandType type, std::string name)
    : type_(type), name_(std::move(name)) {}

Land::Land(LandType type, std::string name, int pass_money)
    : type_(type), name_(std::move(name)), pass_money_(pass_money) {}

int Land::getPurchasePrice(int level) const {
    if (level < 1) return purchase_prices_[0];
    if (level > 4) return purchase_prices_[3];
    return purchase_prices_[static_cast<size_t>(level - 1)];
}

int Land::getToll(int level) const {
    if (level < 1) return tolls_[0];
    if (level > 4) return tolls_[3];
    return tolls_[static_cast<size_t>(level - 1)];
}

int Land::getChainToll(int chain_count) const {
    if (chain_count < 1) return 0;
    size_t idx = static_cast<size_t>(chain_count >= 4 ? 3 : chain_count - 1);
    return chain_tolls_[idx];
}

std::string Land::getDisplayName() const {
    if (type_ == LandType::Chain)
        return "[连锁]" + name_;
    return name_;
}

}  // namespace richman
