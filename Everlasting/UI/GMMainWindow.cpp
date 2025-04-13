#include "GMMainWindow.h"
#include "GMUIManager.h"
#include "../Engine/GMEngine.h"
#include <QKeyEvent>
#include <QScreen>

using namespace GM;

CGMMainWindow::CGMMainWindow(QWidget *parent)
	: QMainWindow(parent), m_pSceneWidget(nullptr),
	m_bInit(false), m_bFull(false), m_bPressed(false),
	m_vPos(QPoint(0,0))
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_Mapped);

	ui.centralWidget->setLayout(ui.centralVLayout);
	ui.titleWidget->setLayout(ui.titleHLayout);
	ui.toolWidget->setLayout(ui.toolGLayout);

	connect(ui.minBtn, SIGNAL(clicked()), this, SLOT(_slotMinimum()));
	connect(ui.maxBtn, SIGNAL(clicked()), this, SLOT(_slotMaximum()));
	connect(ui.closeBtn, SIGNAL(clicked()), this, SLOT(_slotClose()));
	connect(ui.volumeBtn, &QPushButton::clicked, this, &CGMMainWindow::_slotVolume);
	connect(ui.listBtn, &QPushButton::clicked, this, &CGMMainWindow::_slotList);
	connect(ui.fullScreenBtn, SIGNAL(clicked()), this, SLOT(_slotFullScreen()));
	

	// 加载QSS
	QFile qssFile(":/Resources/MainWindow.qss");
	if (qssFile.open(QFile::ReadOnly))
	{
		QString style = QLatin1String(qssFile.readAll());
		setStyleSheet(style);
		qssFile.close();
	}
}

CGMMainWindow::~CGMMainWindow()
{
	GM_UI_MANAGER_PTR->Release();
}

/** @brief 初始化 */
bool CGMMainWindow::Init()
{
	if (m_bInit)
		return true;

	m_pSceneWidget = GM_ENGINE.CreateViewWidget(this);
	ui.centralVLayout->insertWidget(2,(QWidget*)m_pSceneWidget);

	QImage* pAudioImg = new QImage;
	pAudioImg->load(":/Resources/default_Image.png");
	ui.audioImgLab->setPixmap(QPixmap::fromImage(*pAudioImg));

	m_bInit = true;

	return m_bInit;
}

void CGMMainWindow::Update()
{
}

void CGMMainWindow::SetFullScreen(const bool bFull)
{
	if (m_bFull != bFull)
	{
		m_bFull = bFull;

		// 全屏切换
		if (m_bFull)
		{
			QList<QScreen*> mScreen = qApp->screens();
			setGeometry(0, 0, mScreen[0]->geometry().width(), mScreen[0]->geometry().height());
			show();

			ui.titleWidget->hide();
			ui.titleEdgeLab->hide();
			ui.toolWidget->hide();
			ui.toolEdgeLab->hide();
		}
		else
		{
			if (ui.maxBtn->isChecked())
			{
				setGeometry(320, 180, 1280, 720);
				showNormal();
			}
			else
			{
				showMaximized();
			}

			ui.titleWidget->show();
			ui.titleEdgeLab->show();
			ui.toolWidget->show();
			ui.toolEdgeLab->show();
		}
	}
}

bool CGMMainWindow::GetFullScreen()
{
	return m_bFull;
}

void CGMMainWindow::_slotMinimum()
{
	showMinimized();
}

void CGMMainWindow::_slotMaximum()
{
	// 进入这个槽函数时，按钮的状态已经改变
	if (ui.maxBtn->isChecked())
	{
		showNormal();
	}
	else
	{
		showMaximized();
	}
}

void CGMMainWindow::_slotClose()
{
	GM_ENGINE.Save();
	exit(0);
}

void CGMMainWindow::_slotVolume()
{
}

void CGMMainWindow::_slotList()
{
	GM_ENGINE.Play();
}

void CGMMainWindow::_slotFullScreen()
{
	SetFullScreen(true);
}

void CGMMainWindow::changeEvent(QEvent* event)
{
	if (GM_ENGINE.GetRendering() && isMinimized())
	{
		GM_ENGINE.SetRendering(false);
	}
	else if (!GM_ENGINE.GetRendering() && !isMinimized())
	{
		GM_ENGINE.SetRendering(true);
		setAttribute(Qt::WA_Mapped);
	}
	else{}

	QWidget::changeEvent(event);
}

void CGMMainWindow::resizeEvent(QResizeEvent* event)
{
	GM_UI_MANAGER_PTR->Resize();
}

void CGMMainWindow::closeEvent(QCloseEvent* event)
{
	GM_UI_MANAGER_PTR->Release();
}

void CGMMainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->pos().y() < ui.titleWidget->height())
	{
		if (ui.maxBtn->isChecked()) // 窗口化状态
		{
			ui.maxBtn->setChecked(false);
			// 切换到最大化
			showMaximized();
		}
		else // 最大化状态 
		{
			ui.maxBtn->setChecked(true);
			// 切换到窗口
			showNormal();
		}
	}
	QWidget::mouseDoubleClickEvent(event);
}

void CGMMainWindow::mousePressEvent(QMouseEvent * event)
{
	m_bPressed = true;
	m_vPos = event->globalPos();
	QWidget::mousePressEvent(event);
}

void CGMMainWindow::mouseReleaseEvent(QMouseEvent * event)
{
	m_bPressed = false;
	QWidget::mouseReleaseEvent(event);
}

void CGMMainWindow::mouseMoveEvent(QMouseEvent* event)
{
	// 鼠标拖动标题栏以移动窗口
	if (m_bPressed && (event->pos().y() < ui.titleWidget->height()))
	{
		if (ui.maxBtn->isChecked())// 窗口化状态
		{
			QPoint movePoint = event->globalPos() - m_vPos;
			m_vPos = event->globalPos();
			move(x() + movePoint.x(), y() + movePoint.y());
		}
		else // 最大化状态
		{
			// 切换回窗口化状态
			ui.maxBtn->setChecked(true);
			showNormal();

			m_vPos = event->globalPos();
			move(m_vPos.x() - 640, m_vPos.y() - 5);
		}
	}

	QWidget::mouseMoveEvent(event);
}

void CGMMainWindow::keyPressEvent(QKeyEvent* event)
{
	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_S))
	{
		GM_ENGINE.Save();
	}

	switch (event->key())
	{
	case Qt::Key_F2:
	{
	}
	break;
	case Qt::Key_F3:
	{
	}
	break;
	case Qt::Key_F11:
	{
		GM_UI_MANAGER_PTR->SetFullScreen(!GM_UI_MANAGER_PTR->GetFullScreen());
	}
	break;
	default:
		break;
	}

	QWidget::keyPressEvent(event);
}

void CGMMainWindow::keyReleaseEvent(QKeyEvent* event)
{
	QWidget::keyReleaseEvent(event);
}