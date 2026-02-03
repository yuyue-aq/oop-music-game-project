#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSlider>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include "../Core/GameConfig.h"

class SettingsDialog : public QDialog {
    Q_OBJECT

private:
    // UI 控件
    QSlider* noteSpeedSlider;
    QLabel* noteSpeedLabel;

    QDoubleSpinBox* perfectWindowSpinBox;
    QDoubleSpinBox* greatWindowSpinBox;
    QDoubleSpinBox* goodWindowSpinBox;

    QSlider* musicVolumeSlider;
    QLabel* musicVolumeLabel;

    QCheckBox* showAccuracyCheckBox;

    QPushButton* saveButton;
    QPushButton* cancelButton;
    QPushButton* resetButton;

    // 配置
    GameConfig* config;
    GameConfig* tempConfig;  // 临时配置，用于取消时恢复

public:
    explicit SettingsDialog(GameConfig* config, QWidget* parent = nullptr);
    ~SettingsDialog();

signals:
    void configSaved();

private slots:
    void onNoteSpeedChanged(int value);
    void onMusicVolumeChanged(int value);
    void onSaveClicked();
    void onCancelClicked();
    void onResetClicked();

private:
    void setupUI();
    void loadFromConfig();
    void saveToConfig();
};

#endif // SETTINGSDIALOG_H#endif // SETTINGSDIALOG_H