#pragma once
#include <QtWidgets/QMainWindow>
#include <QElapsedTimer>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QTimer>
#include <QKeyEvent>
#include <QPen>
#include <QBrush>
#include <QLinearGradient>
#include <vector>
#include <map>
#include <deque>
#include "ui__QMainWindow.h"
#include "../Core/Note.h"
#include "../Core/Song.h"
#include "../Core/Player.h"
#include "../Core/GameConfig.h"
#include "../Core/HitResult.h"
#include "../Core/OszParser.h"
#include "../UI/ResultDialog.h"
#include "../UI/SettingsDialog.h"
#include "StartWindow.h"

class AudioSync; // 前向声明

class _QMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    _QMainWindow(QWidget* parent = nullptr);
    ~_QMainWindow();
    void showStartWindow();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onStartGame();
    void updateGame();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void selectAndLoadOsz();
    void showResultDialog();
    void showSettingsDialog();
    void showAboutDialog();  // 显示关于对话框

private:
    Ui::_QMainWindowClass ui;
    StartWindow* startWindow;

    // 游戏核心组件
    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;
    QTimer* gameTimer;

    // 游戏数据
    Player* player = nullptr;
    GameConfig* config = nullptr;
    Song* currentSong = nullptr;
    std::vector<Note*> notes;
    OszPackage* currentOsz = nullptr;
    
    std::map<int, std::deque<Note*>> notesByLane;

    // ===== 音频同步相关 =====
    AudioSync* audioSync;
    
    // 优化：一次性预处理同步
    double firstNoteTime;          // 第一个音符的绝对时间戳
    bool isSynced;                 // 是否已完成首次同步
    
    // 修复：添加缺失的音频同步变量
    QElapsedTimer frameTimer;      // 帧计时器
    double smoothedTime = 0.0;     // 平滑后的时间
    qint64 lastFrameTime = 0;      // 上一帧时间
    
    // 游戏状态
    bool isPlaying = false;
    double currentTime = 0.0;
    int laneCount = 4;            // 添加：轨道数量
    int hitLineY = 750; 

    // UI相关
    QFont scoreFont;
    QFont comboFont;
    QFont judgeFont;

    QString lastJudgment;
    int judgmentDisplayTimer = 0;
    
    // 绘制缓存对象（避免重复创建）
    QPen notePen;
    QBrush noteBrush;
    QBrush innerBrush;
    QPen centerLinePen;
    QPen lanePen;
    QLinearGradient hitLineGradient;
    bool hitLineGradientInitialized = false;
    QPen hitLineBorderPen;
    
    // 视觉反馈效果
    std::map<int, int> laneFlashTimer;      // 轨道闪烁计时器 (lane -> frames)
    std::map<Note*, int> noteHitFlashTimer;  // 音符击中闪烁计时器 (note -> frames)
    const int LANE_FLASH_DURATION = 15;      // 轨道闪烁持续帧数 (约250ms @ 60FPS)
    const int NOTE_FLASH_DURATION = 20;      // 音符闪烁持续帧数 (约333ms @ 60FPS)

    // 初始化方法
    void initializeGame();
    void loadSong();
    void loadOszPackage(const std::string& oszPath);
    void buildLaneIndex();
    void preprocessNotes();        // 预处理所有音符
    void clearAllNotes();          // 统一的音符清理方法

    // 游戏逻辑
    void checkHit(int lane);
    HitResult* evaluateHit(Note* note, double hitTime);
    void updateScore(HitResult* result);
    void updateActiveNotes();
    void drawNotes(QPainter& painter);
    void drawUI(QPainter& painter);
    void drawLanes(QPainter& painter);
    void drawHitLine(QPainter& painter);

    // 工具方法
    double getSyncedTime();        // 获取同步后的游戏时间
    int getLaneX(int lane);        // 获取指定轨道的 X 坐标
};