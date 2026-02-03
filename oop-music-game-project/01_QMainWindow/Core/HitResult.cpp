//@孙逸
#include "HitResult.h"
#include <sstream>
#include <iomanip>
#include <cmath>

// ==================== 构造函数实现 ====================
HitResult::HitResult(const std::string& judge, int score, double timeDiff)
    : judgment(judge),
    scoreGain(score),
    timeDifference(timeDiff),
    comboAtHit(0),
    isSpecialNote(false),
    accuracy(0.0),
    noteId(-1)
{
    // 根据时间差计算准确度
    accuracy = calculateAccuracy();
}

HitResult::HitResult(const std::string& judge, int score, double timeDiff,
    int combo, bool special)
    : judgment(judge),
    scoreGain(score),
    timeDifference(timeDiff),
    comboAtHit(combo),
    isSpecialNote(special),
    accuracy(0.0),
    noteId(-1)
{
    // 根据时间差计算准确度
    accuracy = calculateAccuracy();
}

// ==================== 判定类型检查方法实现 ====================
bool HitResult::isPerfect() const {  // 快速判断是否 Perfect
    return judgment == "Perfect";
}

bool HitResult::isGreat() const {
    return judgment == "Great";
}

bool HitResult::isGood() const {
    return judgment == "Good";
}

bool HitResult::isMiss() const {
    return judgment == "Miss";
}

bool HitResult::isBad() const {
    return judgment == "Bad";
}

// ==================== 辅助方法实现 ====================
std::string HitResult::toString() const {
    std::ostringstream oss;
    oss << "HitResult["
        << "Judgment=" << judgment
        << ", Score=" << scoreGain
        << ", TimeDiff=" << std::fixed << std::setprecision(2) << timeDifference << "ms"
        << ", Combo=" << comboAtHit
        << ", Accuracy=" << std::fixed << std::setprecision(1) << (accuracy * 100.0) << "%";

    if (isSpecialNote) {
        oss << ", Special";
    }

    oss << "]";
    return oss.str();
}

double HitResult::calculateAccuracy() const {
    // Miss 判定准确度为 0
    if (isMiss() || isBad()) {
        return 0.0;
    }

    // 根据时间差计算准确度
    // 使用指数衰减函数：accuracy = e^(-|timeDiff| / 50)
    // 时间差越小，准确度越高
    double absDiff = std::abs(timeDifference);

    if (absDiff < 0.1) {
        return 1.0;  // 完美准确度
    }

    // 指数衰减公式
    double acc = std::exp(-absDiff / 50.0);

    // Perfect: 通常在 50ms 内，准确度 > 0.8
    // Great: 50-100ms，准确度 0.5-0.8
    // Good: 100-150ms，准确度 0.3-0.5

    // 限制范围在 [0.0, 1.0]
    if (acc > 1.0) acc = 1.0;
    if (acc < 0.0) acc = 0.0;

    return acc;
}