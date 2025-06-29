//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMViewWidget.h
/// @brief		Galaxy-Music Engine - GMViewWidget.h
/// @version	1.0
/// @author		LiuTao
/// @date		2021.08.15
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "osgQt/GraphicsWindowQt.h"
#include <osgViewer/CompositeViewer>

class CGMViewWidget : public QWidget, public osgViewer::CompositeViewer
{
public:
	CGMViewWidget(osgViewer::View* pView,
		QWidget* parent = 0, Qt::WindowFlags f = 0,
		osgViewer::CompositeViewer::ThreadingModel threadingModel = osgViewer::CompositeViewer::SingleThreaded);
	~CGMViewWidget();

protected:
	osgQt::GraphicsWindowQt* createGraphicsWindow(
		int x, int y, int w, int h,
		const std::string& name = "",
		bool windowDecoration = false);
	/** @brief 界面上的键盘事件 */
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
};