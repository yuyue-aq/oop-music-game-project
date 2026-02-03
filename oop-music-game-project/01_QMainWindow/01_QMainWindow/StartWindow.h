#pragma once

#include <QWidget>
#include<QKeyevent>
#include "ui_StartWindow.h"

class StartWindow : public QWidget
{
	Q_OBJECT

public:
	StartWindow(QWidget *parent = nullptr);
	~StartWindow();
signals:
	void enterPressed();
private:
	Ui::StartWindowClass ui;

protected:
	void keyPressEvent(QKeyEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
};

