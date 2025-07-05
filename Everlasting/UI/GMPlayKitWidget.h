#pragma once

#include <QWidget>
#include "ui_GMPlayKitWidget.h"

class CGMPlayKitWidget : public QWidget
{
	Q_OBJECT

public:
	CGMPlayKitWidget(QWidget *parent = nullptr);
	~CGMPlayKitWidget();

	/** @brief ¸üÐÂ */
	void Update();

private slots:
	/** @brief ²¥·Å/ÔÝÍ£ */
	void _slotPlayOrPause();

private:
	Ui::playKitWidget ui;
};
