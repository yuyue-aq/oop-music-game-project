//@孙逸
#ifndef HITRESULT_H
#define HITRESULT_H

#include <string>

/**
 * @brief 打击判定结果类
 * 记录单次音符打击的详细信息
 */
class HitResult {
private:
    std::string judgment;      // 打击判定（"Perfect"/"Great"/"Good"/"Bad"/"Miss"）
    int scoreGain;             // 本次打击获得的分数
    double timeDifference;     // 打击时间与理想时间的差值（毫秒）
    int comboAtHit;            // 打击时的连击数
    bool isSpecialNote;        // 是否为特殊音符
    double accuracy;           // 单次准确度（0.0-1.0）
    int noteId;                // 相关音符ID

public:
    // ==================== 构造函数 ====================
    /**
     * @brief 基础构造函数
     * @param judge 判定类型
     * @param score 获得分数
     * @param timeDiff 时间差（毫秒）
     */
    HitResult(const std::string& judge, int score, double timeDiff);

    /**
     * @brief 完整构造函数
     * @param judge 判定类型
     * @param score 获得分数
     * @param timeDiff 时间差（毫秒）
     * @param combo 当前连击数
     * @param special 是否特殊音符
     */
    HitResult(const std::string& judge, int score, double timeDiff,
        int combo, bool special);

    // ==================== Getter方法 ====================
    std::string getJudgment() const { return judgment; }
    int getScoreGain() const { return scoreGain; }
    double getTimeDifference() const { return timeDifference; }
    double getTimeDiff() const { return timeDifference; }  // 别名，兼容旧代码
    int getComboAtHit() const { return comboAtHit; }
    bool getIsSpecialNote() const { return isSpecialNote; }
    double getAccuracy() const { return accuracy; }
    int getNoteId() const { return noteId; }

    // ==================== Setter方法 ====================
    void setNoteId(int id) { noteId = id; }
    void setAccuracy(double acc) { accuracy = acc; }

    // ==================== 判定类型检查方法 ====================
    /**
     * @brief 是否为 Perfect 判定
     */
    bool isPerfect() const;

    /**
     * @brief 是否为 Great 判定
     */
    bool isGreat() const;

    /**
     * @brief 是否为 Good 判定
     */
    bool isGood() const;

    /**
     * @brief 是否为 Miss 判定
     */
    bool isMiss() const;

    /**
     * @brief 是否为 Bad 判定
     */
    bool isBad() const;

    // ==================== 辅助方法 ====================
    /**
     * @brief 转换为字符串表示
     * @return 格式化的字符串描述
     */
    std::string toString() const;

    /**
     * @brief 计算并返回准确度
     * @return 准确度值（0.0-1.0）
     */
    double calculateAccuracy() const;
};

#endif // HITRESULT_H