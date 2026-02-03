#include "Player.h"

Player::Player(std::string name)
    : playerName(name),
      score(0),
      combo(0),
      maxCombo(0),
      perfectCount(0),
      greatCount(0),
      goodCount(0),
      badCount(0),
      missCount(0)
{
}

void Player::reset() {
    score = 0;
    combo = 0;
    maxCombo = 0;
    perfectCount = 0;
    greatCount = 0;
    goodCount = 0;
    badCount = 0;
    missCount = 0;
}