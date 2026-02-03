//@张馨予
#include "GameConfig.h"

GameConfig::GameConfig()
    : noteSpeed(500.0),
    judgementLineY(670.0),
    visualOffset(0.0),
    audioOffset(0.0),
    autoPlay(false),
    perfectWindow(50.0),
    greatWindow(100.0),
    goodWindow(150.0),
    badWindow(200.0),
    perfectScore(300),
    greatScore(100),
    goodScore(50),
    volumeMusic(0.5),
    showCombo(true),
    showAccuracy(true)
{
}

void GameConfig::setNoteSpeed(double speed) {
    if (speed > 0) {
        noteSpeed = speed;
    }
}

void GameConfig::setJudgementLineY(double y) {
    judgementLineY = y;
}

void GameConfig::setVisualOffset(double offset) {
    visualOffset = offset;
}

void GameConfig::setAudioOffset(double offset) {
    audioOffset = offset;
}

void GameConfig::setAutoPlay(bool enable) {
    autoPlay = enable;
}

void GameConfig::setPerfectWindow(double window) {
    if (window > 0) {
        perfectWindow = window;
    }
}

void GameConfig::setGreatWindow(double window) {
    if (window > 0) {
        greatWindow = window;
    }
}

void GameConfig::setGoodWindow(double window) {
    if (window > 0) {
        goodWindow = window;
    }
}

void GameConfig::setBadWindow(double window) {
    if (window > 0) {
        badWindow = window;
    }
}

void GameConfig::setPerfectScore(int score) {
    if (score >= 0) {
        perfectScore = score;
    }
}

void GameConfig::setGreatScore(int score) {
    if (score >= 0) {
        greatScore = score;
    }
}

void GameConfig::setGoodScore(int score) {
    if (score >= 0) {
        goodScore = score;
    }
}

void GameConfig::setVolumeMusic(double volume) {
    if (volume >= 0.0 && volume <= 1.0) {
        volumeMusic = volume;
    }
}

void GameConfig::setShowCombo(bool show) {
    showCombo = show;
}

void GameConfig::setShowAccuracy(bool show) {
    showAccuracy = show;
}

void GameConfig::resetToDefault() {
    *this = GameConfig();
}

void GameConfig::copyFrom(const GameConfig& other) {
    noteSpeed = other.noteSpeed;
    judgementLineY = other.judgementLineY;
    visualOffset = other.visualOffset;
    audioOffset = other.audioOffset;
    autoPlay = other.autoPlay;

    perfectWindow = other.perfectWindow;
    greatWindow = other.greatWindow;
    goodWindow = other.goodWindow;
    badWindow = other.badWindow;

    perfectScore = other.perfectScore;
    greatScore = other.greatScore;
    goodScore = other.goodScore;

    volumeMusic = other.volumeMusic;

    showCombo = other.showCombo;
    showAccuracy = other.showAccuracy;
}