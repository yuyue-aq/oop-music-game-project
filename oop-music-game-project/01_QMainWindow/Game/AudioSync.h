#ifndef AUDIOSYNC_H
#define AUDIOSYNC_H

#include "../Core/GameConfig.h"
#include <QMediaPlayer>

/**
 * @brief 音频同步管理器
 * 负责音频播放和时间同步
 * @author 主程序员
 */
class AudioSync {
private:
    QMediaPlayer* mediaPlayer;  // 媒体播放器
    GameConfig* config;         // 游戏配置
    double startTime;           // 游戏开始时间
    bool isPaused;              // 是否暂停
    
public:
    /**
     * @brief 构造函数
     * @param mediaPlayer 媒体播放器指针
     * @param config 游戏配置指针
     */
    AudioSync(QMediaPlayer* mediaPlayer, GameConfig* config);
    
    /**
     * @brief 获取当前音乐时间（考虑偏移）
     * @return 当前时间（毫秒）
     */
    double getCurrentMusicTime() const;
    
    /**
     * @brief 获取原始播放位置
     * @return 原始位置（毫秒）
     */
    double getRawPosition() const;
    
    /**
     * @brief 应用音频偏移
     * @param rawTime 原始时间
     * @return 调整后的时间
     */
    double applyAudioOffset(double rawTime) const;
    
    /**
     * @brief 应用视觉偏移
     * @param rawTime 原始时间
     * @return 调整后的时间
     */
    void play();
    
    /**
     * @brief 暂停音乐
     */
};

#endif // AUDIOSYNC_H