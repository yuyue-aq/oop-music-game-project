#include "StartWindow.h"
#include <QDebug>
#include <QIcon> // 确保包含 QIcon 头文件

StartWindow::StartWindow(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setWindowTitle("Fingertip Rhythm - Start");
    QIcon appIcon("Icon.png");
    this->setWindowIcon(appIcon);
    // 确保窗口能接收键盘输入
    setFocusPolicy(Qt::StrongFocus);
    setWindowIcon(QIcon(":/_QMainWindow/Icon/Icon.png"));
    setFocus();

    // 设置窗口为 1280x720（标准 720p，16:9 比例）
    int targetWidth = 1280;
    int targetHeight = 720;

    resize(targetWidth, targetHeight);

    // 窗口居中显示
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    move((screenGeometry.width() - targetWidth) / 2,
        (screenGeometry.height() - targetHeight) / 2);

    // 确保 overlayWidget 跟随窗口大小调整
    ui.overlayWidget->setGeometry(0, 0, targetWidth, targetHeight);
}

StartWindow::~StartWindow()
{
}

void StartWindow::keyPressEvent(QKeyEvent* event)
{
    qDebug() << "Key pressed:" << event->key();  // 调试输出

    // 同时支持 Enter 和空格键
    if (event->key() == Qt::Key_Return ||
        event->key() == Qt::Key_Enter ||
        event->key() == Qt::Key_Space) {
        qDebug() << "Start key detected, emitting signal";
        emit enterPressed();  // 发射自定义信号,开始游戏
    }

    QWidget::keyPressEvent(event);  // 调用父类实现
}

// 添加 resizeEvent 确保 overlayWidget 始终覆盖整个窗口
void StartWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (ui.overlayWidget) {
        ui.overlayWidget->setGeometry(0, 0, width(), height());
    }
}