#include "_QMainWindow.h"
#include <QPainter>
#include <QDebug>
#include <cmath>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QElapsedTimer>
#include <QDialog>
#include <algorithm>
#include "../Core/Statistics.h"
#include "../Game/AudioSync.h"
#include "ui_aboutDialog.h"

// ==================== 构造函数与析构函数 ====================

// ==================== 构造函数初始化 ====================
_QMainWindow::_QMainWindow(QWidget* parent)
    : QMainWindow(parent),
    mediaPlayer(nullptr),
    audioOutput(nullptr),
    gameTimer(nullptr),
    startWindow(nullptr),
    audioSync(nullptr),
    firstNoteTime(0.0),
    isSynced(false),
    hitLineGradientInitialized(false)
{
    ui.setupUi(this);
    
    // 修复：适配屏幕分辨率
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    
    int targetWidth = qMin(screenWidth, 1920);
    int targetHeight = qMin(screenHeight, 1080);
    
    resize(targetWidth, targetHeight);
    move((screenWidth - targetWidth) / 2, (screenHeight - targetHeight) / 2);
    
    setWindowTitle("Fingertip Rhythm - 指尖律动");
    setWindowIcon(QIcon(":/_QMainWindow/Icon/Icon.png"));
    
    // 初始化绘制对象
    noteBrush.setStyle(Qt::SolidPattern);//全色填充
    innerBrush.setStyle(Qt::SolidPattern);//全色填充
    lanePen.setColor(QColor(100, 100, 120));
    lanePen.setWidth(2);
    hitLineBorderPen.setColor(QColor(255, 200, 0));
    hitLineBorderPen.setWidth(2);
    
    // 初始化渐变
    hitLineGradient.setColorAt(0, QColor(255, 220, 100, 150));
    hitLineGradient.setColorAt(0.5, QColor(255, 200, 0, 255));
    hitLineGradient.setColorAt(1, QColor(255, 220, 100, 150));
    hitLineGradientInitialized = true;
    
    // 不再使用样式表设置背景，改为在 paintEvent 中绘制
    qDebug() << "Background will be drawn in paintEvent";
    
    startWindow = new StartWindow(nullptr);
    startWindow->setWindowFlags(Qt::Window);
    connect(startWindow, &StartWindow::enterPressed, this, &_QMainWindow::onStartGame);
    hide();
}

_QMainWindow::~_QMainWindow()
{
    // 停止游戏，防止在析构过程中还有定时器触发
    if (gameTimer && gameTimer->isActive()) {
        gameTimer->stop();
    }
    if (mediaPlayer) {
        mediaPlayer->stop();
    }

    // Qt对象有父子关系，会自动释放（mediaPlayer, audioOutput, gameTimer）
    // 只删除非Qt对象或者没有父对象的
    if (startWindow) delete startWindow;
    if (player) delete player;
    if (config) delete config;
    if (currentSong) delete currentSong;
    if (currentOsz) OszParser::cleanup(currentOsz);

    // 清除所有音符
    clearAllNotes();

    if (audioSync) delete audioSync;
}

// ==================== 公共方法 ====================

void _QMainWindow::showStartWindow()
{
    startWindow->show();
}

// ==================== 私有槽函数 ====================

void _QMainWindow::onStartGame()
{
    if (!startWindow) return;
    startWindow->hide();
    startWindow->deleteLater();
    startWindow = nullptr;
    show();
    initializeGame();
}

void _QMainWindow::updateGame()
{
    if (!isPlaying) return;

    // 获取音频时间（已应用偏移）
    double audioTime = getSyncedTime();
    
    // 计算帧时间增量
    qint64 currentFrameTime = frameTimer.elapsed();
    double deltaMs = static_cast<double>(currentFrameTime - lastFrameTime);
    lastFrameTime = currentFrameTime;
    
    // 基于帧时间增量更新
    smoothedTime += deltaMs;
    
    // 每帧都做轻微校正（混合 20% 的音频时间，避免跳跃）
    double drift = audioTime - smoothedTime;
    smoothedTime += drift * 0.2;  // 平滑过渡，而不是突然跳跃
    
    // 如果偏差过大（超过 100ms），才强制同步
    if (std::abs(drift) > 100.0) {
        qDebug() << "检测到时间偏差过大:" << drift << "ms，强制同步";
        smoothedTime = audioTime;
        lastFrameTime = frameTimer.elapsed();  // 重置帧时间基准
    }
    
    // 使用平滑后的时间
    currentTime = smoothedTime;

    if (judgmentDisplayTimer > 0) {
        judgmentDisplayTimer--;
    }
    
    // 更新轨道闪烁计时器
    for (auto it = laneFlashTimer.begin(); it != laneFlashTimer.end(); ) {
        if (--it->second <= 0) {
            it = laneFlashTimer.erase(it);
        } else {
            ++it;
        }
    }
    
    // 更新音符击中闪烁计时器，并在闪烁结束后标记音符为已击中
    for (auto it = noteHitFlashTimer.begin(); it != noteHitFlashTimer.end(); ) {
        if (--it->second <= 0) {
            // 闪烁结束后直接移除（音符已经在 checkHit 中标记为 isHit）
            it = noteHitFlashTimer.erase(it);
        } else {
            ++it;
        }
    }

    updateActiveNotes();
    update();
}

void _QMainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        isPlaying = false;
        gameTimer->stop();
        showResultDialog();
    }
    else if (status == QMediaPlayer::LoadedMedia) {
        qDebug() << "Media loaded successfully";
    }
    else if (status == QMediaPlayer::InvalidMedia) {
        qDebug() << "Invalid media!";
        QMessageBox::critical(this, "错误", "无法加载音频文件！");
    }
}

void _QMainWindow::selectAndLoadOsz()
{
    if (isPlaying) {
        isPlaying = false;
        gameTimer->stop();
        mediaPlayer->stop();
    }

    QString fileName = QFileDialog::getOpenFileName(this, "选择 OSZ 文件", "", "OSZ Files (*.osz)");
    if (!fileName.isEmpty()) {
        loadOszPackage(fileName.toStdString());
        currentTime = 0.0;

        // 重置玩家统计
        if (player) {
            player->reset();
        }
    }
}

void _QMainWindow::showResultDialog()
{
    // 创建统计数据
    Statistics stats;
    stats.setTotalNotes(notes.size());
    stats.setPerfectCount(player->getPerfectCount());
    stats.setGreatCount(player->getGreatCount());
    stats.setGoodCount(player->getGoodCount());
    stats.setBadCount(player->getBadCount()); 
    stats.setMissCount(player->getMissCount());
    stats.setMaxCombo(player->getMaxCombo());
    stats.calculateFinalStats();

    // 在栈上创建结算对话框，异常安全
    ResultDialog dialog(&stats, player, this);

    // 连接返回菜单信号 - 清除OSZ数据
    connect(&dialog, &ResultDialog::menuRequested, this, [this]() {
        qDebug() << "Menu requested - Clearing OSZ data and returning to menu";

        // 停止游戏
        isPlaying = false;
        currentTime = 0.0;
        lastJudgment = "";
        judgmentDisplayTimer = 0;

        // 停止播放器和定时器
        if (mediaPlayer) {
            mediaPlayer->stop();
            mediaPlayer->setSource(QUrl());  // 清除音频源
        }
        if (gameTimer) {
            gameTimer->stop();
        }

        // 清除OSZ数据
        if (currentOsz) {
            OszParser::cleanup(currentOsz);
            currentOsz = nullptr;
            qDebug() << "OSZ package cleaned up";
        }

        // 清除歌曲数据
        if (currentSong) {
            delete currentSong;
            currentSong = nullptr;
            qDebug() << "Song data cleared";
        }

        // 清除所有音符
        clearAllNotes();
        qDebug() << "All notes cleared";

        // 重置玩家数据
        player->reset();

        // 刷新界面到待机状态
        update();

        qDebug() << "Game state reset - Ready to load new beatmap";
    });

    // 显示对话框
    dialog.exec();
    // 自动析构
}

void _QMainWindow::showSettingsDialog()
{
    // 在栈上创建对话框
    SettingsDialog dialog(config, this);

    // 连接配置保存信号，实时应用设置
    connect(&dialog, &SettingsDialog::configSaved, this, [this]() {
        qDebug() << "Config saved - Applying settings";

        // 立即应用音量设置
        if (audioOutput) {
            double newVolume = config->getVolumeMusic();
            audioOutput->setVolume(newVolume);
            qDebug() << "Volume updated to:" << newVolume;
        }

        // 更新判定线位置
        hitLineY = static_cast<int>(config->getJudgementLineY());

        // 刷新界面
        update();
        });

    // 显示对话框（模态）
    dialog.exec();
}

void _QMainWindow::showAboutDialog()
{
    // 在栈上创建对话框，自动管理内存
    QDialog dialog(this);
    
    // 设置 UI（使用 UIC 自动生成的类）
    Ui::Dialog ui;
    ui.setupUi(&dialog);
    
    // 显示模态对话框
    dialog.exec();
    
    // 自动析构，无需手动 delete
}

// ==================== 初始化相关 ====================

void _QMainWindow::initializeGame()
{
    // 防止重复初始化造成内存泄漏
    if (player) {
        delete player;
        player = nullptr;
    }
    if (config) {
        delete config;
        config = nullptr;
    }

    // 创建核心对象
    player = new Player("Player1");
    config = new GameConfig();
    
    // 修复：根据窗口高度动态设置判定线位置（85% 位置）
    double judgmentLinePosition = height() * 0.85;
    config->setJudgementLineY(judgmentLinePosition);
    hitLineY = static_cast<int>(judgmentLinePosition);
    
    qDebug() << "Judgment line set at:" << hitLineY << "(85% of" << height() << ")";
    
    // 初始化媒体播放器
    if (!mediaPlayer) {
        mediaPlayer = new QMediaPlayer(this);
        audioOutput = new QAudioOutput(this);
        mediaPlayer->setAudioOutput(audioOutput);

        connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged,
            this, &_QMainWindow::onMediaStatusChanged);
    }
    audioOutput->setVolume(config->getVolumeMusic());

    // 初始化音频同步
    if (!audioSync) {
        audioSync = new AudioSync(mediaPlayer, config);
    }
    
    // 初始化时间平滑变量
    smoothedTime = 0.0;
    frameTimer.start();           // 启动帧计时器
    lastFrameTime = 0;            // 重置为 0（frameTimer.elapsed() 从 0 开始）

    // 初始化游戏定时器 - 保持 60 FPS
    if (!gameTimer) {
        gameTimer = new QTimer(this);
        connect(gameTimer, &QTimer::timeout, this, &_QMainWindow::updateGame);
        gameTimer->setInterval(16);  // 60 FPS
        gameTimer->setTimerType(Qt::PreciseTimer);
    }

    // 初始化字体
    scoreFont.setPixelSize(24);
    scoreFont.setBold(true);
    comboFont.setPixelSize(48);
    comboFont.setBold(true);
    judgeFont.setPixelSize(36);
    judgeFont.setBold(true);

    // 创建菜单栏
    QMenuBar* menuBar = new QMenuBar(this);
    QMenu* gameMenu = menuBar->addMenu(tr("游戏"));

    QAction* settingsAction = gameMenu->addAction(tr("设置"));
    connect(settingsAction, &QAction::triggered, this, &_QMainWindow::showSettingsDialog);

    QAction* exitAction = gameMenu->addAction(tr("退出"));
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    // 创建关于菜单
    QMenu* aboutMenu = menuBar->addMenu(tr("关于"));
    QAction* aboutAction = aboutMenu->addAction(tr("关于本程序"));
    connect(aboutAction, &QAction::triggered, this, &_QMainWindow::showAboutDialog);

    setMenuBar(menuBar);

    // 加载默认歌曲（示例）
    loadSong();
}

void _QMainWindow::loadSong()
{
    // 删除旧的 currentSong
    if (currentSong) {
        delete currentSong;
        currentSong = nullptr;
    }

    // 提示用户加载谱面
    qDebug() << "Waiting for user to load beatmap (Press O)";
}

void _QMainWindow::loadOszPackage(const std::string& oszPath)
{
    // 清理旧数据
    if (currentOsz) {
        OszParser::cleanup(currentOsz);
        currentOsz = nullptr;
    }
    if (currentSong) {
        delete currentSong;
        currentSong = nullptr;
    }

    clearAllNotes();

    // 解析 OSZ 文件
    currentOsz = OszParser::parseOszFile(oszPath);
    if (!currentOsz || currentOsz->beatmaps.empty()) {
        QMessageBox::critical(this, "错误", "无法解析 OSZ 文件！");
        return;
    }

    OsuBeatmap* beatmap = currentOsz->beatmaps[0];
    currentSong = OszParser::createSongFromBeatmap(beatmap, currentOsz->audioFilePath);

    notes = std::move(beatmap->notes);
    beatmap->notes.clear();

    buildLaneIndex();
    preprocessNotes();  // 添加预处理调用

    // 根据BPM自动设置音符速度
    double bpm = currentSong->getBpm();
    if (bpm > 0) {
        const double BASE_BPM = 120.0;
        const double BASE_SPEED = 500.0;
        double recommendedSpeed = BASE_SPEED * (bpm / BASE_BPM);

        if (recommendedSpeed < 200.0) recommendedSpeed = 200.0;
        if (recommendedSpeed > 1000.0) recommendedSpeed = 1000.0;

        config->setNoteSpeed(recommendedSpeed);

        qDebug() << "Auto-set note speed to" << recommendedSpeed
            << "based on BPM" << bpm;
    }

    mediaPlayer->setSource(QUrl::fromLocalFile(QString::fromStdString(currentOsz->audioFilePath)));

    qDebug() << "Loaded OSZ:" << QString::fromStdString(currentSong->getTitle());
    qDebug() << "BPM:" << currentSong->getBpm();
    qDebug() << "Notes:" << notes.size();
    qDebug() << "Note Speed:" << config->getNoteSpeed();

    QMessageBox::information(this, "加载成功",
        QString("谱面：%1\n艺术家：%2\nBPM：%3\n音符数：%4\n推荐速度：%5")
        .arg(QString::fromStdString(currentSong->getTitle()))
        .arg(QString::fromStdString(currentSong->getArtist()))
        .arg(currentSong->getBpm(), 0, 'f', 1)
        .arg(notes.size())
        .arg(config->getNoteSpeed(), 0, 'f', 0));
}

// ==================== 新增：预处理所有音符 ====================
void _QMainWindow::preprocessNotes()
{
    if (notes.empty()) {
        firstNoteTime = 0.0;
        qDebug() << "No notes to preprocess";
        return;
    }

    // 找到最早的音符时间（作为同步基准）
    firstNoteTime = notes[0]->getTimestamp();
    for (const auto& note : notes) {
        if (note->getTimestamp() < firstNoteTime) {
            firstNoteTime = note->getTimestamp();
        }
    }

    qDebug() << "========== 音符预处理 ==========";
    qDebug() << "总音符数:" << notes.size();
    qDebug() << "第一个音符时间:" << firstNoteTime << "ms";
    qDebug() << "音符时间戳保持原始值";
    qDebug() << "等待游戏开始时进行首次同步";
    qDebug() << "==============================";
}

// ==================== 修改：获取同步时间（移除错误的偏移逻辑）====================
double _QMainWindow::getSyncedTime()
{
    double rawTime = audioSync->getCurrentMusicTime();
    
    // 首次同步检测（仅用于日志记录）
    if (!isSynced && rawTime >= firstNoteTime - 100.0) {
        isSynced = true;
        
        qDebug() << "========== 首次同步完成 ==========";
        qDebug() << "第一个音符时间:" << firstNoteTime << "ms";
        qDebug() << "当前音频时间:" << rawTime << "ms";
        qDebug() << "音符时间戳已正确对齐，无需额外偏移";
        qDebug() << "==================================";
    }
    
    // 直接返回音频时间（音符时间戳已经是相对于音频开始的绝对时间）
    return rawTime;
}

// ====================drawNotes 使用简化计算 ====================
void _QMainWindow::drawNotes(QPainter& painter)
{
    if (!isPlaying || notes.empty() || !currentSong) return;

    const int noteWidth = 100;
    const int noteHeight = 30;
    const double SCROLL_TIME = 2000.0;
    const double ACCELERATION_FACTOR = 1.10;
    const double badWindow = config->getBadWindow();

    const int screenTop = 0;
    const int noteStartY = screenTop - 300;
    const int totalDistance = hitLineY - noteStartY;
    const double baseSpeed = static_cast<double>(totalDistance) / SCROLL_TIME;
    const double speedMultiplier = config->getNoteSpeed() / 500.0;
    
    const int visibleTop = -noteHeight;
    /*把可见区域上边界扩展到窗口顶部之上一个音符高度，
    允许那些刚刚从屏幕上方进入或部分可见的音符被绘制。
    否则音符在刚开始进入屏幕的那一帧可能被裁剪掉。*/
    const int visibleBottom = height() + noteHeight;//底部
    
    painter.setClipRect(0, visibleTop, width(), visibleBottom - visibleTop);
    painter.setClipping(true);
    painter.setRenderHint(QPainter::Antialiasing, false);
    
    std::vector<std::pair<Note*, int>> visibleNotes;
    visibleNotes.reserve(50);
    
    // 直接计算时间差，无需额外检查
    for (auto note : notes) {
        // 如果音符正在闪烁，继续绘制（即使已标记为击中）说明正在播放击中闪烁效果。
        bool isFlashing = (noteHitFlashTimer.find(note) != noteHitFlashTimer.end());
        
        if (note->getIsHit() && !isFlashing) continue;

        double timeDiff = note->getTimestamp() - currentTime;
        
        // 如果正在闪烁，强制显示在判定线位置
        if (isFlashing) {
            timeDiff = 0.0;  // 固定在判定线上
        }
        
        if (timeDiff < -badWindow || timeDiff > SCROLL_TIME) continue;

        // 计算 Y 位置（保持原有的加速效果）
        double distanceRatio = std::max(0.0, std::min(1.0, timeDiff / SCROLL_TIME));
        double acceleration = 1.0 + (ACCELERATION_FACTOR - 1.0) * distanceRatio;
        double currentSpeed = baseSpeed * speedMultiplier * acceleration;
        double fallDistance = timeDiff * currentSpeed;
        int y = hitLineY - static_cast<int>(fallDistance);

        if (y < visibleTop || y > visibleBottom) continue;
        
        visibleNotes.emplace_back(note, y);
    }
    
    // 绘制逻辑 - 使用成员变量避免重复创建
    for (const auto& [note, y] : visibleNotes) {
        int laneX = getLaneX(note->getLane());

        int alphaValue = 255;
        if (y < 100) {
            double alpha = static_cast<double>(y + 300) / 400.0;
            alphaValue = static_cast<int>(std::max(0.0, std::min(1.0, alpha)) * 255);
        }

        int alpha180 = (alphaValue * 180) / 255;
        int alpha200 = (alphaValue * 200) / 255;
        int alpha220 = (alphaValue * 220) / 255;

        notePen.setColor(QColor(255, 100, 200, alphaValue));
        notePen.setWidth(3);
        painter.setPen(notePen);
        noteBrush.setColor(QColor(255, 150, 220, alpha180));
        painter.setBrush(noteBrush);
        painter.drawRect(laneX - noteWidth / 2, y - noteHeight / 2, noteWidth, noteHeight);

        const int innerWidth = 85;
        const int innerHeight = 18;
        
        // 检查是否有击中闪烁效果
        bool hasFlash = (noteHitFlashTimer.find(note) != noteHitFlashTimer.end());
        
        if (hasFlash) {
            // 击中时闪烁为亮白色
            int flashFrames = noteHitFlashTimer[note];
            int flashAlpha = static_cast<int>((static_cast<double>(flashFrames) / NOTE_FLASH_DURATION) * 255);
            innerBrush.setColor(QColor(255, 255, 255, std::min(flashAlpha, alphaValue)));
        } else {
            // 正常颜色
            innerBrush.setColor(QColor(255, 200, 240, alpha200));
        }
        
        painter.setBrush(innerBrush);
        painter.drawRect(laneX - innerWidth / 2, y - innerHeight / 2, innerWidth, innerHeight);

        centerLinePen.setColor(QColor(255, 255, 255, alpha220));
        centerLinePen.setWidth(2);
        painter.setPen(centerLinePen);
        painter.drawLine(laneX, y - 10, laneX, y + 10);
    }

    painter.setClipping(false);
}

// ==================== 修改：开始游戏时重置同步状态 ====================
void _QMainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && !isPlaying) {
        showSettingsDialog();
        return;
    }

    if (event->key() == Qt::Key_O && !isPlaying) {
        selectAndLoadOsz();
        return;
    }

    if (event->key() == Qt::Key_Space && !isPlaying) {
        if (!currentSong) {
            QMessageBox::warning(this, "提示", "请先加载谱面！");
            return;
        }
        if (mediaPlayer->mediaStatus() != QMediaPlayer::LoadedMedia &&
            mediaPlayer->mediaStatus() != QMediaPlayer::BufferedMedia) {
            QMessageBox::warning(this, "提示", "音频加载中，请稍候...");
            return;
        }
        
        // 重置所有时间相关变量
        currentTime = 0.0;
        smoothedTime = 0.0;
        isSynced = false;
        
        // 重启帧计时器
        frameTimer.restart();
        lastFrameTime = 0;
        
        // 清除视觉效果
        laneFlashTimer.clear();
        noteHitFlashTimer.clear();
        
        isPlaying = true;
        gameTimer->start();
        audioSync->play();
        
        qDebug() << "游戏开始 - 启用平滑混合同步";
        qDebug() << "预期第一个音符时间:" << firstNoteTime << "ms";
        return;
    }

    if (!isPlaying) return;

    int lane = -1;
    if (event->key() == Qt::Key_D) lane = 1;
    else if (event->key() == Qt::Key_F) lane = 2;
    else if (event->key() == Qt::Key_J) lane = 3;
    else if (event->key() == Qt::Key_K) lane = 4;

    if (lane != -1 && !event->isAutoRepeat()) {
        // 触发轨道闪烁效果
        laneFlashTimer[lane] = LANE_FLASH_DURATION;
        
        checkHit(lane);
    }

    QMainWindow::keyPressEvent(event);
}

// ==================== 工具方法 ====================

int _QMainWindow::getLaneX(int lane)
{
    const int laneWidth = 150;
    const int startX = (width() - laneWidth * laneCount) / 2;
    return startX + (lane - 1) * laneWidth + laneWidth / 2;
}

// ==================== 绘制方法 ====================

void _QMainWindow::drawLanes(QPainter& painter)
{
    const int laneWidth = 150;
    const int startX = (width() - laneWidth * laneCount) / 2;
    
    // 先绘制轨道闪烁效果（作为背景）
    for (const auto& [lane, framesLeft] : laneFlashTimer) {
        int laneX = startX + (lane - 1) * laneWidth;
        
        // 计算淡出效果 (从 80 逐渐变为 0)
        int alpha = static_cast<int>((static_cast<double>(framesLeft) / LANE_FLASH_DURATION) * 80);
        QColor flashColor(200, 200, 255, alpha);  // 淡蓝色闪烁
        
        painter.fillRect(laneX, 0, laneWidth, height(), flashColor);
    }
    
    // 绘制轨道分隔线 - 使用成员变量
    painter.setPen(lanePen);
    for (int i = 0; i <= laneCount; i++) {
        int x = startX + i * laneWidth;
        painter.drawLine(x, 0, x, height());
    }
}

void _QMainWindow::drawHitLine(QPainter& painter)
{
    const int laneWidth = 150;
    const int startX = (width() - laneWidth * laneCount) / 2;
    const int totalWidth = laneWidth * laneCount;
    const int rectHeight = 8;

    // 更新渐变位置 - 使用成员变量
    hitLineGradient.setStart(startX, hitLineY - rectHeight / 2);
    hitLineGradient.setFinalStop(startX, hitLineY + rectHeight / 2);

    painter.setPen(Qt::NoPen);
    painter.setBrush(hitLineGradient);
    painter.drawRect(startX, hitLineY - rectHeight / 2, totalWidth, rectHeight);

    // 边框 - 使用成员变量
    painter.setPen(hitLineBorderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(startX, hitLineY - rectHeight / 2, totalWidth, rectHeight);
}

void _QMainWindow::drawUI(QPainter& painter)
{
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    
    // 使用窗口尺寸的百分比而非固定坐标
    int margin = width() * 0.01;  // 1% 边距
    int topMargin = height() * 0.065;  // 约 6.5% 顶部边距
    
    // 绘制分数（左上角）
    painter.setFont(scoreFont);
    painter.setPen(Qt::white);
    painter.drawText(margin, topMargin, QString("Score: %1").arg(player->getScore()));

    // 绘制连击数（右上角）
    if (player->getCombo() > 0) {
        painter.setFont(comboFont);
        painter.setPen(QColor(255, 200, 0));
        QString comboText = QString("%1 COMBO").arg(player->getCombo());
        QFontMetrics fm(comboFont);
        int textWidth = fm.horizontalAdvance(comboText);
        painter.drawText(width() - textWidth - margin, topMargin, comboText);
    }

    // 其余统计信息和判定显示保持相对位置
    if (config->isShowAccuracy()) {
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        int statsY = static_cast<int>(height() * 0.09);  // 相对位置
        
        int totalNotes = player->getPerfectCount() + player->getGreatCount() + 
                        player->getGoodCount() + player->getBadCount() + player->getMissCount();
        if (totalNotes > 0) {
            double weightedHits = player->getPerfectCount() * 1.0 + 
                                 player->getGreatCount() * 0.8 + 
                                 player->getGoodCount() * 0.5+
                                    player->getBadCount()*0.2;
            double accuracy = (weightedHits / totalNotes) * 100.0;
            
            painter.setPen(QColor(100, 255, 255));
            painter.drawText(margin, statsY, QString("Accuracy: %1%").arg(accuracy, 0, 'f', 2));
            statsY += 25;
        }
        
        painter.setFont(QFont("Arial", 12));
        painter.setPen(Qt::white);
        painter.drawText(margin, statsY, QString("Perfect: %1").arg(player->getPerfectCount()));
        painter.drawText(margin, statsY + 20, QString("Great: %1").arg(player->getGreatCount()));
        painter.drawText(margin, statsY + 40, QString("Good: %1").arg(player->getGoodCount()));
        painter.drawText(margin, statsY + 60, QString("Bad: %1").arg(player->getBadCount()));
        painter.drawText(margin, statsY + 80, QString("Miss: %1").arg(player->getMissCount()));
    }

    // 判定结果显示在判定线上方（相对于判定线）
    if (judgmentDisplayTimer > 0) {
        painter.setFont(judgeFont);
        
        static const QColor perfectColor(255, 215, 0);
        static const QColor greatColor(0, 255, 0);
        static const QColor goodColor(0, 150, 255);
        static const QColor badColor(255, 100, 0);
        static const QColor missColor(255, 0, 0);
        
        QColor judgeColor;
        if (lastJudgment == "Perfect") judgeColor = perfectColor;
        else if (lastJudgment == "Great") judgeColor = greatColor;
        else if (lastJudgment == "Good") judgeColor = goodColor;
        else if (lastJudgment == "Bad") judgeColor = badColor;
        else judgeColor = missColor;

        painter.setPen(judgeColor);
        
        // 相对于判定线的位置
        QFontMetrics fm(judgeFont);
        int textWidth = fm.horizontalAdvance(lastJudgment);
        painter.drawText((width() - textWidth) / 2, hitLineY - 100, lastJudgment);
    }

    // 提示信息居中显示
    if (!isPlaying) {
        int centerY = height() / 2 - 100;
        
        QFont hintFont("Arial", 28, QFont::Bold);
        painter.setFont(hintFont);
        
        QString text = "按 O 键加载谱面";
        QRect textRect(0, centerY, width(), 50);
        
        painter.setPen(QColor(0, 0, 0, 200));
        painter.drawText(textRect.adjusted(2, 2, 2, 2), Qt::AlignCenter, text);
        painter.setPen(QColor(255, 255, 0));
        painter.drawText(textRect, Qt::AlignCenter, text);
        
        centerY += 60;
        
        QString text2 = "按空格键开始游戏";
        QRect textRect2(0, centerY, width(), 50);
        
        painter.setPen(QColor(0, 0, 0, 200));
        painter.drawText(textRect2.adjusted(3, 3, 3, 3), Qt::AlignCenter, text2);
        painter.setPen(QColor(100, 255, 100));
        painter.drawText(textRect2, Qt::AlignCenter, text2);
        
        centerY += 70;
        
        QFont tipFont("Arial", 20);
        painter.setFont(tipFont);
        
        QString tip1 = "D F J K 打击音符";
        QRect tip1Rect(0, centerY, width(), 40);
        painter.setPen(QColor(0, 0, 0, 180));
        painter.drawText(tip1Rect.adjusted(2, 2, 2, 2), Qt::AlignCenter, tip1);
        painter.setPen(QColor(220, 220, 220));
        painter.drawText(tip1Rect, Qt::AlignCenter, tip1);
        
        centerY += 45;
        
        QString tip2 = "按 ESC 打开设置";
        QRect tip2Rect(0, centerY, width(), 40);
        painter.setPen(QColor(0, 0, 0, 180));
        painter.drawText(tip2Rect.adjusted(2, 2, 2, 2), Qt::AlignCenter, tip2);
        painter.setPen(QColor(220, 220, 220));
        painter.drawText(tip2Rect, Qt::AlignCenter, tip2);
    }

    // 歌曲信息（左下角，使用相对坐标）
    if (currentSong && !isPlaying) {
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.setPen(Qt::white);
        
        int infoY = height() - 110;
        painter.drawText(20, infoY, QString("歌曲：%1").arg(QString::fromStdString(currentSong->getTitle())));
        painter.drawText(20, infoY + 25, QString("艺术家：%1").arg(QString::fromStdString(currentSong->getArtist())));
        painter.drawText(20, infoY + 50, QString("BPM：%1").arg(currentSong->getBpm(), 0, 'f', 1));
        painter.drawText(20, infoY + 75, QString("音符数：%1").arg(notes.size()));
    }
}

// ==================== 游戏逻辑方法 ====================

void _QMainWindow::clearAllNotes()
{
    for (auto note : notes) {
        delete note;
    }
    notes.clear();
    notesByLane.clear();
}

void _QMainWindow::buildLaneIndex()
{
    notesByLane.clear();
    
    for (auto note : notes) {
        int lane = note->getLane();
        notesByLane[lane].push_back(note);
    }
    
    // 对每个轨道的音符按时间排序
    for (auto& [lane, laneNotes] : notesByLane) {
        std::sort(laneNotes.begin(), laneNotes.end(), 
            [](Note* a, Note* b) { return a->getTimestamp() < b->getTimestamp(); });
    }
    
    qDebug() << "Lane index built - Total lanes:" << notesByLane.size();
}

void _QMainWindow::checkHit(int lane)
{
    if (!config || notesByLane[lane].empty()) return;

    const double badWindow = config->getBadWindow();
    
    // 获取该轨道的第一个未击中的音符
    Note* targetNote = nullptr;
    for (auto note : notesByLane[lane]) {
        if (!note->getIsHit()) {
            targetNote = note;
            break;
        }
    }
    
    if (!targetNote) return;

    // 计算时间差
    double timeDiff = targetNote->getTimestamp() - currentTime;
    
    // 判断是否在有效击打窗口内
    if (std::abs(timeDiff) <= badWindow) {
        HitResult* result = evaluateHit(targetNote, currentTime);
        
        std::string judgment = result->getJudgment();
        qDebug() << "Hit! Lane:" << lane << "| Judgment:" << QString::fromStdString(judgment);
        
        // 修复：立即标记为已击中，防止 updateActiveNotes 重复判定
        targetNote->setIsHit(true);
        
        // 如果击中成功（不是 Miss 和 Bad），触发音符闪烁效果
        if (judgment == "Perfect" || judgment == "Great" || judgment == "Good") {
            noteHitFlashTimer[targetNote] = NOTE_FLASH_DURATION;
        }
        
        updateScore(result);
        delete result;
    }
}

HitResult* _QMainWindow::evaluateHit(Note* note, double hitTime)
{
    if (!config) return new HitResult("Miss", 0, 0.0);

    double timeDiff = std::abs(note->getTimestamp() - hitTime);
    
    const double perfectWindow = config->getPerfectWindow();
    const double greatWindow = config->getGreatWindow();
    const double goodWindow = config->getGoodWindow();
    const double badWindow = config->getBadWindow();

    std::string judgment;
    int score;
    
    if (timeDiff <= perfectWindow) {
        judgment = "Perfect";
        score = 300;
        lastJudgment = "Perfect";
    }
    else if (timeDiff <= greatWindow) {
        judgment = "Great";
        score = 200;
        lastJudgment = "Great";
    }
    else if (timeDiff <= goodWindow) {
        judgment = "Good";
        score = 100;
        lastJudgment = "Good";
    }
    else if (timeDiff <= badWindow) {
        judgment = "Bad";
        score = 50;
        lastJudgment = "Bad";
    }
    else {
        judgment = "Miss";
        score = 0;
        lastJudgment = "Miss";
    }
    
    judgmentDisplayTimer = 30; // 显示半秒（30帧 @ 60FPS）
    
    return new HitResult(judgment, score, timeDiff);
}

void _QMainWindow::updateScore(HitResult* result)
{
    if (!player || !result) return;
    
    player->addScore(result->getScoreGain());
    
    std::string judgment = result->getJudgment();
    
    if (judgment == "Perfect") {
        player->incrementPerfectCount();
        player->incrementCombo();
    }
    else if (judgment == "Great") {
        player->incrementGreatCount();
        player->incrementCombo();
    }
    else if (judgment == "Good") {
        player->incrementGoodCount();
        player->incrementCombo();
    }
    else if (judgment == "Bad") {
        player->incrementBadCount();
        player->resetCombo();
    }
    else if (judgment == "Miss") {
        player->incrementMissCount();
        player->resetCombo();
    }
}

void _QMainWindow::updateActiveNotes()
{
    if (!config) return;
    
    const double missWindow = config->getBadWindow();
    
    // 检查是否有音符超过判定窗口（Miss）
    for (auto note : notes) {
        if (note->getIsHit()) continue;
        
        double timeDiff = note->getTimestamp() - currentTime;
        
        // 音符已经过了判定线太远，判定为 Miss
        if (timeDiff < -missWindow) {
            note->setIsHit(true);
            player->incrementMissCount();
            player->resetCombo();
            lastJudgment = "Miss";
            judgmentDisplayTimer = 30;
        }
    }
}

// ==================== 绘制事件 ====================

void _QMainWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    // 第一步：绘制半透明背景图
    // 加载背景图
    static QPixmap bgImage(":/_QMainWindow/background.png");
    if (!bgImage.isNull()) {
        // 缩放背景图以适应窗口
        QPixmap scaledBg = bgImage.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        
        // 居中绘制
        int x = (width() - scaledBg.width()) / 2;
        int y = (height() - scaledBg.height()) / 2;
        
        // 设置不透明度并绘制背景
        painter.setOpacity(0.3);  // 30% 不透明度（背景很淡）
        painter.drawPixmap(x, y, scaledBg);
        
        // 恢复不透明度
        painter.setOpacity(1.0);
    }
    
    //第二步：绘制半透明黑色遮罩（进一步降低背景亮度）
    painter.fillRect(rect(), QColor(0, 0, 0, 100));  // 更淡的遮罩
    
    //第三步：绘制游戏元素（会在遮罩之上，清晰可见）
    drawLanes(painter);
    drawHitLine(painter);
    drawNotes(painter);
    drawUI(painter);
    
    QMainWindow::paintEvent(event);
}