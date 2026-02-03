#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "../Core/Statistics.h"
#include "../Core/Player.h"

/**
 * @brief 游戏结算对话框
 * 显示游戏结果和统计信息
 * @author 余越
 */
class ResultDialog : public QDialog {
    Q_OBJECT

private:
    // UI 组件
    QLabel* titleLabel;
    QLabel* scoreLabel;
    QLabel* accuracyLabel;
    QLabel* comboLabel;
    QLabel* rankLabel;
    QLabel* judgeStatsLabel;
    QPushButton* menuButton;
    
    // 数据
    Statistics* stats;
    Player* player;

public:
    /**
     * @brief 构造函数
     * @param stats 统计数据
     * @param player 玩家数据
     * @param parent 父窗口
     */
    ResultDialog(Statistics* stats, Player* player, QWidget* parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~ResultDialog();

signals:
    /**
     * @brief 返回菜单信号
     */
    void menuRequested();

private slots:
    /**
     * @brief 菜单按钮点击
     */
    void onMenuClicked();

private:
    /**
     * @brief 初始化UI
     */
    void setupUI();
    
    /**
     * @brief 更新显示内容
     */
    void updateDisplay();
    
    /**
     * @brief 获取等级颜色
     * @param rank 等级
     * @return 颜色字符串
     */
    QString getRankColor(const std::string& rank) const;
};

#endif // RESULTDIALOG_H