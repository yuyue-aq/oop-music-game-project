//@张馨予
#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <string>

class GameConfig {
private:
    // 游戏配置
    double noteSpeed;        // 音符速度倍率（音符是固定距离/秒）
    double judgementLineY;   // 判定线Y坐标位置
    double visualOffset;     // 视觉偏移（毫秒）
    double audioOffset;      // 音频偏移（毫秒）
    bool autoPlay;           // 自动游玩模式
    
    // 判定窗口（毫秒）
    double perfectWindow;    // Perfect判定窗口
    double greatWindow;      // Great判定窗口
    double goodWindow;       // Good判定窗口
    double badWindow;        // Bad判定窗口（超过即Miss判定）
    
    // 分数配置
    int perfectScore;        // Perfect判定单次得分
    int greatScore;          // Great判定单次得分
    int goodScore;           // Good判定单次得分
    
    // 音量设置（0.0-1.0）
    double volumeMusic;      // 背景音乐音量
    
    // 视觉设置
    bool showCombo;          // 显示连击数
    bool showAccuracy;       // 显示准确率

public:
    /**
     * @brief 构造函数，初始化默认值。
     */
    GameConfig();
    
    // ==================== Getter方法 ====================
    double getNoteSpeed() const { return noteSpeed; }
    double getJudgementLineY() const { return judgementLineY; }
    double getVisualOffset() const { return visualOffset; }
    double getAudioOffset() const { return audioOffset; }
    bool isAutoPlay() const { return autoPlay; }
    
    double getPerfectWindow() const { return perfectWindow; }
    double getGreatWindow() const { return greatWindow; }
    double getGoodWindow() const { return goodWindow; }
    double getBadWindow() const { return badWindow; }
    
    int getPerfectScore() const { return perfectScore; }
    int getGreatScore() const { return greatScore; }
    int getGoodScore() const { return goodScore; }
    
    double getVolumeMusic() const { return volumeMusic; }
    
    bool isShowCombo() const { return showCombo; }
    bool isShowAccuracy() const { return showAccuracy; }

    // ==================== Setter方法（包含验证） ====================
    void setNoteSpeed(double speed);
    void setJudgementLineY(double y);
    void setVisualOffset(double offset);
    void setAudioOffset(double offset);
    void setAutoPlay(bool enable);
    
    void setPerfectWindow(double window);
    void setGreatWindow(double window);
    void setGoodWindow(double window);
    void setBadWindow(double window);
    
    void setPerfectScore(int score);
    void setGreatScore(int score);
    void setGoodScore(int score);
    
    void setVolumeMusic(double volume);
    
    void setShowCombo(bool show);
    void setShowAccuracy(bool show);
    
    // ==================== 配置管理方法（SettingsDialog需要） ====================
    /**
     * @brief 重置为默认配置
     */
    void resetToDefault();
    
    /**
     * @brief 复制另一个配置对象
     * @param other 源对象
     */
    void copyFrom(const GameConfig& other);
};

#endif // GAMECONFIG_H