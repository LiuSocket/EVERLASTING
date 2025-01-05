
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
#include <QtGui>
#include <QtCore/QTimer>
#include <QWidget>
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
	QWidget* widget1 = gw->getGLWidget();
	QGridLayout* grid = new QGridLayout;
	grid->addWidget(widget1, 0, 0);
	setLayout(grid);
	grid->setMargin(0);
	grid->setSpacing(0);

	osg::Camera* camera = pView->getCamera();
	camera->setGraphicsContext(gw);
}

CGMViewWidget::~CGMViewWidget()
{
	_timer.stop();
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

void CGMViewWidget::enterEvent(QEvent* event)
{
	emit _signalEnter3D();
}