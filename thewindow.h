#pragma once

#include <QtWidgets/QWidget>
#include "ui_thewindow.h"

class TheWindow : public QWidget
{
	Q_OBJECT

public:
	TheWindow(QWidget *parent = Q_NULLPTR);

private:
	Ui::TheWindowClass ui;
};
