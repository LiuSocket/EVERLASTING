#include "GMMainWindow.h"
#include "GMVolumeWidget.h"
#include "GMPlayKitWidget.h"
#include "../Engine/GMEngine.h"
#include <QKeyEvent>
#include <QScreen>
#include <QMenu>

using namespace GM;

CGMMainWindow::CGMMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_Mapped);

	qApp->installEventFilter(this); // 对全局应用安装过滤器

	ui.centralWidget->setLayout(ui.centralVLayout);
	ui.titleWidget->setLayout(ui.titleHLayout);
	ui.toolWidget->setLayout(ui.toolGLayout);

	connect(ui.lastBtn, SIGNAL(clicked()), this, SLOT(_slotLast()));
	connect(ui.playBtn, SIGNAL(clicked()), this, SLOT(_slotPlayOrPause()));
	connect(ui.nextBtn, SIGNAL(clicked()), this, SLOT(_slotNext()));

	connect(ui.minBtn, SIGNAL(clicked()), this, SLOT(_slotMinimum()));
	connect(ui.maxBtn, SIGNAL(clicked()), this, SLOT(_slotMaximum()));
	connect(ui.closeBtn, SIGNAL(clicked()), this, SLOT(_slotClose()));

	connect(ui.timeSlider, SIGNAL(valueChanged(int)), this, SLOT(_slotSetAudioTime(int)));

	connect(ui.volumeBtn, SIGNAL(clicked()), this, SLOT(_slotSetMute()));
	connect(ui.fullScreenBtn, SIGNAL(clicked()), this, SLOT(_slotFullScreen()));

	// 音量控件
	m_pVolumeWidget = new CGMVolumeWidget(this);
	m_pVolumeWidget->raise();
	m_pVolumeWidget->hide();
	connect(m_pVolumeWidget, SIGNAL(_signalSetVolume(int)), this, SLOT(_slotSetVolume(int)));

	// 播放器工具控件
	m_pPlayKitWidget = new CGMPlayKitWidget(this);
	m_pPlayKitWidget->move(GetSystemMetrics(SM_CXSCREEN) - 300, GetSystemMetrics(SM_CYSCREEN) - 200);
	m_pPlayKitWidget->raise();
	m_pPlayKitWidget->hide();

	// 加载QSS
	QFile qssFile(":/Resources/MainWindow.qss");
	if (qssFile.open(QFile::ReadOnly))
	{
		QString style = QLatin1String(qssFile.readAll());
		setStyleSheet(style);
		m_pVolumeWidget->setStyleSheet(style);
		qssFile.close();
	}

	// 系统托盘图标
	QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(QIcon(":/Resources/GM_logo.ico"));
	trayIcon->setToolTip("EVERLASTING");
	// 添加右键菜单
	QMenu* trayMenu = new QMenu(this);
	trayMenu->addAction(QString::fromLocal8Bit("播放器"), m_pPlayKitWidget, SLOT(show()));
	trayMenu->addAction(QString::fromLocal8Bit("退出"), qApp, SLOT(quit()));
	trayIcon->setContextMenu(trayMenu);
	trayIcon->show();

	connect(trayIcon, &QSystemTrayIcon::activated, this, &CGMMainWindow::_OnTrayIconActivated);
}

CGMMainWindow::~CGMMainWindow()
{
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

	// 设置成桌面壁纸
	if (GM_ENGINE.IsWallpaper())
	{
		SetFullScreen(true);
		HWND hwnd = (HWND)winId();
		HWND desktopHwnd = _GetDesktopHWND();
		SetParent(hwnd, desktopHwnd);
		// 去除窗口装饰
		LONG style = GetWindowLong(hwnd, GWL_STYLE);
		style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
		SetWindowLong(hwnd, GWL_STYLE, style);
		// 设置全屏
		SetWindowPos(hwnd, HWND_TOP,
			0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			SWP_SHOWWINDOW | SWP_FRAMECHANGED);
	}

	m_bInit = true;
	return m_bInit;
}

void CGMMainWindow::Update()
{
	// 更新音量
	if (m_bShowVolume)
	{
		m_pVolumeWidget->SetVolume(GM_ENGINE.GetVolume() * 100);
	}

	if (GM_ENGINE.IsWallpaper())
	{
		// 壁纸模式下，需要在这里更新鼠标位置
		POINT pt;
		GetCursorPos(&pt);
		// pt.x, pt.y 就是当前鼠标的屏幕坐标
		QPoint globalPos(pt.x, pt.y);
		QPoint localPos = mapFromGlobal(globalPos);
		GM_ENGINE.SetLookTargetPos(SGMVector2f(localPos.x(), GetSystemMetrics(SM_CYSCREEN) - localPos.y()));

		// 更新mini播放控件
		m_pPlayKitWidget->Update();
	}
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

			ui.listBtn->setChecked(false);
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

void CGMMainWindow::UpdateAudioInfo()
{
	const std::wstring wstrAudioName = GM_ENGINE.GetAudioName();
	QString strFileName = QString::fromStdWString(wstrAudioName);
	if ("" == strFileName || GM_ENGINE.IsAudioOver())
	{
		// 将播放/暂停按钮设置成暂停状态
		ui.playBtn->setChecked(false);
		return;
	}
	if (m_strName != strFileName)
	{
		m_strName = strFileName;
		strFileName.chop(strFileName.size() - strFileName.lastIndexOf("."));
		QStringList strList = strFileName.split(" - ");
		// 如果切成的段数大于1，第一段就是作者名，第二段就是歌曲名称
		if (1 < strList.size())
		{
			ui.audioNameTextScroller->setText(strList[1]);
			ui.audioInfoTextScroller->setText(strList[0]);
		}
		else
		{
			ui.audioNameTextScroller->setText(strFileName);
			ui.audioInfoTextScroller->setText("Unknown");
		}

		m_iAudioDuration = GM_ENGINE.GetAudioDuration();

		// 将播放/暂停按钮设置成播放状态
		ui.playBtn->setChecked(true);

		// 计算并显示当前音频总时长
		int iMinutesAll = 0;
		int iSecondsAll = 0;
		_Million2MinutesSeconds(m_iAudioDuration, iMinutesAll, iSecondsAll);
		QString strAll = QString::number(iMinutesAll);
		if (iMinutesAll < 10) strAll = "0" + strAll;
		strAll += ":";
		if (iSecondsAll < 10) strAll += "0";
		strAll += QString::number(iSecondsAll);
		ui.timeAllLab->setText(strAll);
	}
	// 获取当前音频播放位置，单位：ms
	int iCurrentTime = GM_ENGINE.GetAudioCurrentTime();
	float fTimeRatio = 400 * float(iCurrentTime) / float(m_iAudioDuration);
	// 避免循环修改时间
	int iTimeLast = ui.timeSlider->value();
	if(abs(fTimeRatio - iTimeLast) > 0.5f)
		ui.timeSlider->setValue(fTimeRatio);

	// 计算并显示已播放时间
	int iMinutesPassed = 0;
	int iSecondsPassed = 0;
	_Million2MinutesSeconds(iCurrentTime, iMinutesPassed, iSecondsPassed);
	QString strPassed = QString::number(iMinutesPassed);
	if (iMinutesPassed < 10) strPassed = "0" + strPassed;
	strPassed += ":";
	if (iSecondsPassed < 10) strPassed += "0";
	strPassed += QString::number(iSecondsPassed);
	ui.timePassedLab->setText(strPassed);
}

void CGMMainWindow::SetVolumeVisible(const bool bVisible)
{
	if (bVisible)
	{
		int iX = pos().x() + ui.volumeBtn->pos().x();
		int iY = pos().y() + ui.toolEdgeLab->pos().y() - m_pVolumeWidget->size().height() + 20;
		m_pVolumeWidget->move(iX, iY);
	}

	m_pVolumeWidget->setVisible(bVisible);
	m_bShowVolume = bVisible;
}

void CGMMainWindow::_slotLast()
{
	//GM_ENGINE.Last();
	ui.playBtn->setChecked(true);
}

void CGMMainWindow::_slotPlayOrPause()
{
	if (ui.playBtn->isChecked())
	{
		GM_ENGINE.Play();
	}
	else
	{
		GM_ENGINE.Pause();
	}
}

void CGMMainWindow::_slotNext()
{
	GM_ENGINE.Next();
	ui.playBtn->setChecked(true);
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

void CGMMainWindow::_slotSetAudioTime(int iTimeRatio)
{
	// 当前音频播放的时刻
	int iAudioCurrentTime = float(iTimeRatio)*0.0025*m_iAudioDuration;
	int iAudioCurrentPreciseTime = GM_ENGINE.GetAudioCurrentTime();
	if (std::abs(iAudioCurrentPreciseTime - iAudioCurrentTime) > 1000)
	{
		GM_ENGINE.SetAudioCurrentTime(iAudioCurrentTime);
	}
}

void CGMMainWindow::_slotSetMute()
{
	if (ui.volumeBtn->isChecked())
	{
		GM_ENGINE.SetVolume(0.0f);
	}
	else
	{
		GM_ENGINE.SetVolume(m_pVolumeWidget->GetVolume()*0.01f);
	}
}

void CGMMainWindow::_slotSetVolume(int iVolume)
{
	if (0 == iVolume)
	{
		if(!ui.volumeBtn->isChecked())
			ui.volumeBtn->setChecked(true);
	}
	else
	{
		if (ui.volumeBtn->isChecked())
			ui.volumeBtn->setChecked(false);
	}

	GM_ENGINE.SetVolume(iVolume*0.01f);
}

void CGMMainWindow::_slotFullScreen()
{
	SetFullScreen(true);
}

void CGMMainWindow::_OnTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger) // 单击
	{
		if (m_pPlayKitWidget->isVisible())
		{
			m_pPlayKitWidget->hide();
		}
		else
		{
			m_pPlayKitWidget->raise();
			m_pPlayKitWidget->show();
		}
	}
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
	m_pVolumeWidget->hide();
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
	// 设置音量控件显隐
	int iVolumeMinX = ui.volumeBtn->pos().x();
	int iVolumeMaxX = iVolumeMinX + ui.volumeBtn->width();
	int iVolumeMinY = ui.toolEdgeLab->pos().y();
	int iVolumeMaxY = iVolumeMinY + ui.volumeBtn->height();

	if (event->x() < iVolumeMinX || event->x() > iVolumeMaxX ||
		event->y() < iVolumeMinY || event->y() > iVolumeMaxY)
	{
		if (m_pVolumeWidget->isVisible())
		{
			m_pVolumeWidget->hide();
		}
	}
	else if (!m_pVolumeWidget->isVisible())
	{
		int iX = pos().x() + ui.volumeBtn->pos().x();
		int iY = pos().y() + ui.toolEdgeLab->pos().y() - m_pVolumeWidget->size().height() + 20;

		m_pVolumeWidget->SetVolume(GM_ENGINE.GetVolume() * 100);
		m_pVolumeWidget->move(iX, iY);
		m_pVolumeWidget->show();
	}

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

bool CGMMainWindow::eventFilter(QObject* obj, QEvent* event)
{
	if (!GM_ENGINE.IsWallpaper() && event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

		if ((keyEvent->modifiers() == Qt::ControlModifier) && (keyEvent->key() == Qt::Key_S))
		{
			GM_ENGINE.Save();
			return true; // 事件已处理，不再传递
		}

		switch (keyEvent->key())
		{
		case Qt::Key_F2:
		{
			GM_ENGINE.SetPlayMode(EGMA_MOD_CIRCLE);
			GM_ENGINE.Next();
			return true; // 事件已处理，不再传递
		}
		break;
		case Qt::Key_F3:
		{
			GM_ENGINE.SetPlayMode(EGMA_MOD_RANDOM);
			GM_ENGINE.Next();
			return true; // 事件已处理，不再传递
		}
		break;
		case Qt::Key_F11:
		{
			SetFullScreen(!m_bFull);
			return true; // 事件已处理，不再传递
		}
		break;
		case Qt::Key_Escape:
		{
			SetFullScreen(false);
			return true; // 事件已处理，不再传递
		}
		break;
		default:
			break;
		}
	}
	// 其他事件交给基类处理
	return QMainWindow::eventFilter(obj, event);
}

void CGMMainWindow::_Million2MinutesSeconds(const int ms, int & minutes, int & seconds)
{
	int iAllSeconds = ms / 1000;
	minutes = max(0, min(59, iAllSeconds / 60));
	seconds = max(0, min(59, iAllSeconds % 60));
}

HWND CGMMainWindow::_GetDesktopHWND()
{
	HWND progman = FindWindow(L"Progman", NULL);
	// 向Progman发送消息，创建WorkerW
	SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);

	// 24H2 下，SHELLDLL_DefView 和 WorkerW 的关系不再固定，桌面窗口层级更复杂。
	// 开发动态壁纸时，必须动态查找实际的桌面父窗口，不能硬编码假设。
	HWND desktop = NULL;
	EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
		HWND shellView = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);
		if (shellView != NULL)
		{
			HWND* pDesktop = (HWND*)lParam;
			*pDesktop = hwnd;
			return FALSE;
		}
		// 递归查找子窗口
		HWND child = FindWindowEx(hwnd, NULL, NULL, NULL);
		while (child) {
			HWND shellView = FindWindowEx(child, NULL, L"SHELLDLL_DefView", NULL);
			if (shellView != NULL)
			{
				HWND* pDesktop = (HWND*)lParam;
				*pDesktop = child;
				return FALSE;
			}
			child = FindWindowEx(hwnd, child, NULL, NULL);
		}
		return TRUE;
		}, (LPARAM)&desktop);

	if (desktop != NULL)
	{
		HWND workerw = FindWindowEx(NULL, desktop, L"WorkerW", NULL);
		if (workerw)
			return workerw;
	}
	return progman;
}