#include "_QMainWindow.h"
#include "StartWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    _QMainWindow mainWindow;
    mainWindow.showStartWindow();  // 添加这一行来显示启动窗口
    
    return app.exec();
}
