
#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player {
private:
    std::string playerName;  // 玩家名称
    int score;               // 当前总分
    int combo;               // 当前连击数
    int maxCombo;            // 最大连击数 - 添加这行
    int perfectCount;        // Perfect判定次数
    int greatCount;          // Great判定次数
    int goodCount;           // Good判定次数
    int badCount;            // Bad判定次数
    int missCount;           // Miss判定次数

public:
    // 构造函数
    Player(std::string name);
    
    // Getter方法
    int getScore() const { return score; }
    int getCombo() const { return combo; }
    int getMaxCombo() const { return maxCombo; }  // 添加这行
    int getPerfectCount() const { return perfectCount; }
    int getGreatCount() const { return greatCount; }
    int getGoodCount() const { return goodCount; }
    int getBadCount() const { return badCount; }
    int getMissCount() const { return missCount; }
    
    // 计数增加方法（统一使用 increment 命名）
    void incrementPerfectCount() { perfectCount++; }
    void incrementGreatCount() { greatCount++; }
    void incrementGoodCount() { goodCount++; }
    void incrementBadCount() { badCount++; }
    void incrementMissCount() { missCount++; }
    void incrementCombo() { combo++; if (combo > maxCombo) maxCombo = combo; }
    void resetCombo() { combo = 0; }
    
    // 增加分数
    void addScore(int s) { score += s; }
    
    // Reset方法
    void reset();
};

#endif // PLAYER_H