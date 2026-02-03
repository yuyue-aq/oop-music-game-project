//@孙逸
#include "Statistics.h"
#include <sstream>
#include <iomanip>
#include <cmath>

// ==================== 构造函数 ====================
Statistics::Statistics()
    : totalNotes(0),
      perfectCount(0),
      greatCount(0),
      goodCount(0),
	  badCount(0),
      missCount(0),
      maxCombo(0),
	  accuracy(0.0),
      totalScore(0)
{
}

void Statistics::calculateFinalStats() {
    // 计算准确率
    accuracy = calculateAccuracy();
    
    // 计算最终得分（基础分 + 连击加成）
    totalScore = calculateFinalScore();
}

// ==================== 计算方法 ====================
double Statistics::calculateAccuracy() const {
    if (totalNotes == 0) {
        return 0.0;
    }
    
    // 加权准确率计算
    // Perfect = 1.0, Great = 0.8, Good = 0.5, Miss = 0.0
    double weightedScore = perfectCount * 1.0 +
        greatCount * 0.8 +
        goodCount * 0.5 +
        badCount * 0.2;
    
    return weightedScore / totalNotes;
}

std::string Statistics::getRank() const {
    if (accuracy >= 0.95) return "S";
    if (accuracy >= 0.90) return "A";
    if (accuracy >= 0.80) return "B";
    if (accuracy >= 0.70) return "C";
    if (accuracy >= 0.60) return "D";
    return "F";
}

int Statistics::calculateFinalScore() const {
    // Combo 加成计算
        // 基础分按判定计算（与 evaluateHit 中使用的分值保持一致）
    // Perfect = 300, Great = 200, Good = 100, Bad = 50
    int baseScore = perfectCount * 300 +
        greatCount * 200 +
        goodCount * 100 +
        badCount * 50;
    int bonus = maxCombo * 10; // 每个连击加 10 分
    return baseScore+bonus;
}

bool Statistics::isFullCombo() const {
    return (missCount == 0) && (totalNotes > 0);
}