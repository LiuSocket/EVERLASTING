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

#include "osgQOpenGL/osgQOpenGLWidget.h"

class CGMViewWidget : public osgQOpenGLWidget
{
	Q_OBJECT;

public:
	CGMViewWidget(QWidget* parent = 0);
	~CGMViewWidget();
};