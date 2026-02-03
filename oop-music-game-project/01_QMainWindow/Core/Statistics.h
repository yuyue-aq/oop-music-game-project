//@孙逸
#ifndef STATISTICS_H
#define STATISTICS_H

#include <string>
#include "HitResult.h"

/**
 * @brief 游戏统计类
 * 记录和计算游戏过程中的各项统计数据
 * @author 成员 B
 */
class Statistics {
private:
    // 基础统计（8个）
    int totalNotes;          // 总音符数
    int perfectCount;        // Perfect 次数
    int greatCount;          // Great 次数
    int goodCount;           // Good 次数
	int badCount;            // Bad 次数
    int missCount;           // Miss 次数
    int maxCombo;            // 最大连击数
    
    // 分数统计（3个）
    int totalScore;          // 总分
    
    // 时间统计（3个）
    double accuracy;         // 准确率（0.0-1.0）
    

public:
    // ==================== 构造函数 ====================
    /**
     * @brief 默认构造函数
     */
    Statistics();
    
    // ==================== Getter方法 ====================
    int getTotalNotes() const { return totalNotes; }
    int getPerfectCount() const { return perfectCount; }
    int getGreatCount() const { return greatCount; }
    int getGoodCount() const { return goodCount; }
    int getMissCount() const { return missCount; }
    int getBadCount()const { return badCount; }
    int getMaxCombo() const { return maxCombo; }
    
    int getTotalScore() const { return totalScore; }
    
    double getAccuracy() const { return accuracy; }
    
    // ==================== Setter方法 ====================
    void setTotalNotes(int total) { totalNotes = total; }
    void setTotalScore(int score) { totalScore = score; }
    
    // 添加以下 setter 方法
    void setPerfectCount(int count) { perfectCount = count; }
    void setGreatCount(int count) { greatCount = count; }
    void setGoodCount(int count) { goodCount = count; }
    void setBadCount(int count) { badCount = count; }
    void setMissCount(int count) { missCount = count; }
    void setMaxCombo(int combo) { maxCombo = combo; }

    // ==================== 更新方法 ====================
    
    
    /**
     * @brief 计算最终统计数据
     */
    void calculateFinalStats();
    
    // ==================== 计算方法 ====================
    /**
     * @brief 计算准确率
     * @return 准确率（0.0-1.0）
     */
    double calculateAccuracy() const;
    
    /**
     * @brief 获取评级（S/A/B/C/D/F）
     * @return 评级字符串
     */
    std::string getRank() const;
    
    /**
     * @brief 计算最终得分
     * @return 最终分数
     */
    int calculateFinalScore() const;
    
    /**
     * @brief 是否全连击
     * @return 是否 Full Combo
     */
    bool isFullCombo() const;
};

#endif  STATISTICS_H