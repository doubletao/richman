#include "Player.h"

namespace richman {

Player::Player(int initial_money, std::string name)
    : name_(std::move(name)), money_(initial_money), position_(0), freeze_rounds_(0) {}

void Player::tickRound() {
    if (freeze_rounds_ > 0)
        --freeze_rounds_;
}

}  // namespace richman
