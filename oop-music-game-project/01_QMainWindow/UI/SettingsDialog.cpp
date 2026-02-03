#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>

SettingsDialog::SettingsDialog(GameConfig* config, QWidget* parent)
    : QDialog(parent), config(config)
{
    // 创建临时配置（用于取消操作）
    tempConfig = new GameConfig();
    tempConfig->copyFrom(*config);

    setupUI();
    loadFromConfig();
}

SettingsDialog::~SettingsDialog() {
    delete tempConfig;
}

void SettingsDialog::setupUI() {
    setWindowTitle(tr("游戏设置"));
    setFixedSize(400, 350);
    setModal(true);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    // 游戏设置组
    QGroupBox* gameGroup = new QGroupBox(tr("游戏设置"), this);
    QFormLayout* gameLayout = new QFormLayout(gameGroup);

    // 音符速度
    QHBoxLayout* speedLayout = new QHBoxLayout();
    noteSpeedSlider = new QSlider(Qt::Horizontal, this);
    noteSpeedSlider->setMinimum(200);
    noteSpeedSlider->setMaximum(1000);
    noteSpeedSlider->setSingleStep(50);
    connect(noteSpeedSlider, &QSlider::valueChanged,
        this, &SettingsDialog::onNoteSpeedChanged);

    noteSpeedLabel = new QLabel("500", this);
    noteSpeedLabel->setFixedWidth(50);

    speedLayout->addWidget(noteSpeedSlider);
    speedLayout->addWidget(noteSpeedLabel);
    gameLayout->addRow(tr("音符速度:"), speedLayout);

    mainLayout->addWidget(gameGroup);

    // 音量设置组
    QGroupBox* audioGroup = new QGroupBox(tr("音量设置"), this);
    QFormLayout* audioLayout = new QFormLayout(audioGroup);

    // 音乐音量
    QHBoxLayout* musicVolLayout = new QHBoxLayout();
    musicVolumeSlider = new QSlider(Qt::Horizontal, this);
    musicVolumeSlider->setMinimum(0);
    musicVolumeSlider->setMaximum(100);
    connect(musicVolumeSlider, &QSlider::valueChanged,
        this, &SettingsDialog::onMusicVolumeChanged);

    musicVolumeLabel = new QLabel("50%", this);
    musicVolumeLabel->setFixedWidth(50);

    musicVolLayout->addWidget(musicVolumeSlider);
    musicVolLayout->addWidget(musicVolumeLabel);
    audioLayout->addRow(tr("背景音乐:"), musicVolLayout);

    mainLayout->addWidget(audioGroup);

    // 显示设置组
    QGroupBox* displayGroup = new QGroupBox(tr("显示设置"), this);
    QVBoxLayout* displayLayout = new QVBoxLayout(displayGroup);

    showAccuracyCheckBox = new QCheckBox(tr("显示准确率"), this);

    displayLayout->addWidget(showAccuracyCheckBox);

    mainLayout->addWidget(displayGroup);

    mainLayout->addStretch();

    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    saveButton = new QPushButton(tr("保存"), this);
    cancelButton = new QPushButton(tr("取消"), this);
    resetButton = new QPushButton(tr("重置默认"), this);

    connect(saveButton, &QPushButton::clicked, this, &SettingsDialog::onSaveClicked);
    connect(cancelButton, &QPushButton::clicked, this, &SettingsDialog::onCancelClicked);
    connect(resetButton, &QPushButton::clicked, this, &SettingsDialog::onResetClicked);

    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(saveButton);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void SettingsDialog::loadFromConfig() {
    noteSpeedSlider->setValue(static_cast<int>(config->getNoteSpeed()));
    noteSpeedLabel->setText(QString::number(static_cast<int>(config->getNoteSpeed())));

    musicVolumeSlider->setValue(static_cast<int>(config->getVolumeMusic() * 100));
    musicVolumeLabel->setText(QString::number(static_cast<int>(config->getVolumeMusic() * 100)) + "%");

    showAccuracyCheckBox->setChecked(config->isShowAccuracy());
}

void SettingsDialog::saveToConfig() {
    config->setNoteSpeed(noteSpeedSlider->value());
    config->setVolumeMusic(musicVolumeSlider->value() / 100.0);
    config->setShowAccuracy(showAccuracyCheckBox->isChecked());
}

void SettingsDialog::onNoteSpeedChanged(int value) {
    noteSpeedLabel->setText(QString::number(value));
}

void SettingsDialog::onMusicVolumeChanged(int value) {
    musicVolumeLabel->setText(QString::number(value) + "%");
}

void SettingsDialog::onSaveClicked() {
    saveToConfig();
    emit configSaved();
    accept();
}

void SettingsDialog::onCancelClicked() {
    // 恢复原配置
    config->copyFrom(*tempConfig);
    reject();
}

void SettingsDialog::onResetClicked() {
    config->resetToDefault();
    loadFromConfig();
}