
//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMViewWidget.cpp
/// @brief		Galaxy-Music Engine - GMViewWidget.cpp
/// @version	1.0
/// @author		LiuTao
/// @date		2021.08.15
//////////////////////////////////////////////////////////////////////////

#include "GMViewWidget.h"
#include <QWidget>
#include <QKeyEvent>
#include <QApplication>
#include <QGridLayout>

CGMViewWidget::CGMViewWidget(osgViewer::View* pView,QWidget* parent, Qt::WindowFlags f,
	osgViewer::CompositeViewer::ThreadingModel threadingModel)
	: QWidget(parent, f)
{
	setThreadingModel(threadingModel);
	setKeyEventSetsDone(0);
	setQuitEventSetsDone(false);
	addView(pView);

	osgQt::GraphicsWindowQt* gw = createGraphicsWindow(0, 0, 100, 100);
	osgQt::GLWidget* pGLWidget = gw->getGLWidget();

	QGridLayout* grid = new QGridLayout;
	setLayout(grid);

	grid->addWidget(pGLWidget, 0, 0);
	grid->setMargin(0);
	grid->setSpacing(0);

	osg::Camera* camera = pView->getCamera();
	camera->setGraphicsContext(gw);
}

CGMViewWidget::~CGMViewWidget()
{
	stopThreading();
}

osgQt::GraphicsWindowQt* CGMViewWidget::createGraphicsWindow(
	int x, int y, int w, int h,
	const std::string & name,
	bool windowDecoration)
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowName = name;
	traits->windowDecoration = windowDecoration;
	traits->x = x;
	traits->y = y;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = ds->getNumMultiSamples();

	return new osgQt::GraphicsWindowQt(traits.get());
}

void CGMViewWidget::keyPressEvent(QKeyEvent* event)
{
	// 子窗口自己的处理逻辑

	// 传递给父窗口
	if (parentWidget())
	{
		QApplication::sendEvent(parentWidget(), event);
	}
	else
	{
		QWidget::keyPressEvent(event);
	}
}

void CGMViewWidget::keyReleaseEvent(QKeyEvent* event)
{
	// 子窗口自己的处理逻辑

	// 传递给父窗口
	if (parentWidget())
	{
		QApplication::sendEvent(parentWidget(), event);
	}
	else
	{
		QWidget::keyPressEvent(event);
	}
}