#include "ResultDialog.h"
#include <QFont>
#include <sstream>
#include <iomanip>

ResultDialog::ResultDialog(Statistics* stats, Player* player, QWidget* parent)
    : QDialog(parent), stats(stats), player(player)
{
    setupUI();
    updateDisplay();
}

ResultDialog::~ResultDialog() {
    // UI组件会被Qt自动删除
}

void ResultDialog::setupUI() {
    setWindowTitle(QString::fromLocal8Bit("游戏结算"));
    setFixedSize(600, 600);  // 适度增加宽度
    setModal(true);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    
    // 标题
    titleLabel = new QLabel(QString::fromLocal8Bit("游戏结束"), this);
    QFont titleFont;
    titleFont.setPixelSize(36);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    mainLayout->addSpacing(20);
    
    // 等级显示
    rankLabel = new QLabel(this);
    QFont rankFont;
    rankFont.setPixelSize(72);
    rankFont.setBold(true);
    rankLabel->setFont(rankFont);
    rankLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(rankLabel);
    
    // 分数
    scoreLabel = new QLabel(this);
    QFont scoreFont;
    scoreFont.setPixelSize(28);
    scoreLabel->setFont(scoreFont);
    scoreLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(scoreLabel);
    
    // 准确率
    accuracyLabel = new QLabel(this);
    QFont statFont;
    statFont.setPixelSize(20);
    accuracyLabel->setFont(statFont);
    accuracyLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(accuracyLabel);
    
    // 最大连击
    comboLabel = new QLabel(this);
    comboLabel->setFont(statFont);
    comboLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(comboLabel);
    
    mainLayout->addSpacing(10);
    
    // 判定统计 - 使用更小字体和两行布局
    judgeStatsLabel = new QLabel(this);
    QFont judgeFont;
    judgeFont.setPixelSize(15);  // 稍微缩小
    judgeStatsLabel->setFont(judgeFont);
    judgeStatsLabel->setAlignment(Qt::AlignCenter);
    judgeStatsLabel->setWordWrap(false);  // 手动控制换行
    mainLayout->addWidget(judgeStatsLabel);
    
    mainLayout->addStretch();
    
    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20);
    
    menuButton = new QPushButton(QString::fromLocal8Bit("返回菜单"), this);
    menuButton->setFixedSize(200, 50);
    QFont buttonFont;
    buttonFont.setPixelSize(18);
    menuButton->setFont(buttonFont);
    connect(menuButton, &QPushButton::clicked, this, &ResultDialog::onMenuClicked);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(menuButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
}

void ResultDialog::updateDisplay() {
    if (!stats || !player) return;
    
    // 更新等级
    std::string rank = stats->getRank();
    QString rankColor = getRankColor(rank);
    rankLabel->setText(QString::fromStdString(rank));
    rankLabel->setStyleSheet(QString("QLabel { color: %1; }").arg(rankColor));
    
    // 更新分数
    int score = stats->getTotalScore();
    scoreLabel->setText(QString::fromLocal8Bit("得分: %1").arg(score));
    
    // 更新准确率
    double accuracy = stats->getAccuracy() * 100.0;
    accuracyLabel->setText(QString::fromLocal8Bit("准确率: %1%")
        .arg(QString::number(accuracy, 'f', 2)));
    
    // 更新连击
    int maxCombo = stats->getMaxCombo();
    QString comboText = QString::fromLocal8Bit("最大连击: %1").arg(maxCombo);
    if (stats->isFullCombo()) {
        comboText += QString::fromLocal8Bit(" (FULL COMBO!)");
    }
    comboLabel->setText(comboText);
    
    // 更新判定统计 - 两行显示
    QString judgeStats = QString::fromLocal8Bit(
        "Perfect: %1  |  Great: %2  |  Good: %3\n"
        "Bad: %4  |  Miss: %5"
    ).arg(stats->getPerfectCount())
        .arg(stats->getGreatCount())
        .arg(stats->getGoodCount())
        .arg(stats->getBadCount())
        .arg(stats->getMissCount());
    
    judgeStatsLabel->setText(judgeStats);
}

QString ResultDialog::getRankColor(const std::string& rank) const {
    if (rank == "S") return "#FFD700";  // 金色
    if (rank == "A") return "#00FF00";  // 绿色
    if (rank == "B") return "#00BFFF";  // 蓝色
    if (rank == "C") return "#FFA500";  // 橙色
    if (rank == "D") return "#FF6347";  // 番茄红
    return "#808080";  // 灰色 (F)
}

void ResultDialog::onMenuClicked() {
    emit menuRequested();
    accept();//隐藏模态对话框并将结果代码设置为Accepted。
}