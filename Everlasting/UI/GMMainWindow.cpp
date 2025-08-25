#include "GMMainWindow.h"
#include "GMVolumeWidget.h"
#include "GMPlayKitWidget.h"
#include "../Engine/GMEngine.h"

#include <QKeyEvent>
#include <QScreen>
#include <QMenu>

#include <atlbase.h> // Add this include to define CComPtr
#include <ShObjIdl.h>
#include <dwmapi.h>
#include <psapi.h>
#include <functional>
#include <thread>
#include <chrono>

using namespace GM;

/*************************************************************************
 CGMMainWindow Methods
*************************************************************************/
CGMMainWindow::CGMMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_b24H2OrGreater = _Is24H2OrGreater(); // 检查当前操作系统是否是win11 24H2版本或更高版本

	// 如果设置无边框模式，就没办法在win11 24H2上实现动态壁纸
	if (!GM_ENGINE.IsWallpaper() || !m_b24H2OrGreater)
	{
		setWindowFlags(Qt::FramelessWindowHint);
		setAttribute(Qt::WA_Mapped);
	}

	qApp->installEventFilter(this); // 对全局应用安装过滤器

	ui.centralWidget->setLayout(ui.centralVLayout);
	ui.titleWidget->setLayout(ui.titleHLayout);
	ui.toolWidget->setLayout(ui.toolGLayout);

	if (GM_ENGINE.IsWallpaper())
	{
		//// 播放器工具控件
		//m_pPlayKitWidget = new CGMPlayKitWidget();
		//QList<QScreen*> mScreen = qApp->screens();
		//m_pPlayKitWidget->move(mScreen[0]->geometry().width()-300, mScreen[0]->geometry().height()-200);
		//m_pPlayKitWidget->raise();
		//m_pPlayKitWidget->hide();

		// 系统托盘图标
		m_pTrayIcon = new QSystemTrayIcon(this);
		m_pTrayIcon->setIcon(QIcon(":/Resources/GM_logo.ico"));
		m_pTrayIcon->setToolTip("EVERLASTING");
		// 添加右键菜单
		QMenu* trayMenu = new QMenu(this);
		//trayMenu->addAction(QString::fromLocal8Bit("播放器"), m_pPlayKitWidget, SLOT(show()));
		m_pWallpaperPlayAct = trayMenu->addAction(QString::fromLocal8Bit("播放音乐"), this, SLOT(_slotWallpaperPlayOrPause()));
		trayMenu->addAction(QString::fromLocal8Bit("退出"), this, SLOT(_slotClose()));
		m_pTrayIcon->setContextMenu(trayMenu);
		m_pTrayIcon->show();

		//connect(trayIcon, &QSystemTrayIcon::activated, this, &CGMMainWindow::_OnTrayIconActivated);
	}
	else
	{
		connect(ui.lastBtn, SIGNAL(clicked()), this, SLOT(_slotLast()));
		connect(ui.playBtn, SIGNAL(clicked()), this, SLOT(_slotPlayOrPause()));
		connect(ui.nextBtn, SIGNAL(clicked()), this, SLOT(_slotNext()));

		connect(ui.minBtn, SIGNAL(clicked()), this, SLOT(_slotMinimum()));
		connect(ui.maxBtn, SIGNAL(clicked()), this, SLOT(_slotMaximum()));
		connect(ui.closeBtn, SIGNAL(clicked()), this, SLOT(_slotClose()));

		connect(ui.timeSlider, SIGNAL(valueChanged(int)), this, SLOT(_slotSetAudioTime(int)));

		connect(ui.volumeBtn, SIGNAL(clicked()), this, SLOT(_slotSetMute()));
		connect(ui.fullScreenBtn, SIGNAL(clicked()), this, SLOT(_slotFullScreen()));
	}

	// 音量控件
	m_pVolumeWidget = new CGMVolumeWidget(this);
	m_pVolumeWidget->raise();
	m_pVolumeWidget->hide();
	connect(m_pVolumeWidget, SIGNAL(_signalSetVolume(int)), this, SLOT(_slotSetVolume(int)));

	// 加载QSS
	QFile qssFile(":/Resources/MainWindow.qss");
	if (qssFile.open(QFile::ReadOnly))
	{
		QString style = QLatin1String(qssFile.readAll());
		setStyleSheet(style);
		m_pVolumeWidget->setStyleSheet(style);
		qssFile.close();
	}
}

CGMMainWindow::~CGMMainWindow()
{
}

/** @brief 初始化 */
bool CGMMainWindow::Init()
{
	if (m_bInit)
		return true;

	QImage* pAudioImg = new QImage;
	pAudioImg->load(":/Resources/default_Image.png");
	ui.audioImgLab->setPixmap(QPixmap::fromImage(*pAudioImg));

	m_pSceneWidget = GM_ENGINE.CreateViewWidget(this);
	if (GM_ENGINE.IsWallpaper())// 设置成桌面壁纸
	{
		QList<QScreen*> mScreen = qApp->screens();
		m_pSceneWidget->setGeometry(
			mScreen[0]->geometry().topLeft().x(),
			mScreen[0]->geometry().topLeft().y(),
			mScreen[0]->geometry().width(),
			mScreen[0]->geometry().height());

		SetFullScreen(true);
		_SetWallPaper((HWND)winId());
	}
	else
	{
		ui.centralVLayout->insertWidget(2, (QWidget*)m_pSceneWidget);
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
		//m_pPlayKitWidget->Update();

		// 更新音频相关信息，后续会放到间隔更新函数中
		const std::wstring wstrAudioName = GM_ENGINE.GetAudioName();
		QString strFileName = QString::fromStdWString(wstrAudioName);
		if ("" == strFileName || GM_ENGINE.IsAudioOver())
		{
			// 更新按钮
			m_pWallpaperPlayAct->setText(QString::fromLocal8Bit("播放音乐"));
			m_bPlayingMusic = false;
		}
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
			if (!(GM_ENGINE.IsWallpaper() && m_b24H2OrGreater))
			{
				QList<QScreen*> mScreen = qApp->screens();
				setGeometry(
					mScreen[0]->geometry().topLeft().x(),
					mScreen[0]->geometry().topLeft().y(),
					mScreen[0]->geometry().width(),
					mScreen[0]->geometry().height());
			}
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
		m_bPlayingMusic = false;
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

void CGMMainWindow::UpdateWallpaper()
{
	if (_IsOtherAppFullscreen())
	{
		GM_ENGINE.SetRendering(false);
	}
	else
	{
		GM_ENGINE.SetRendering(true);
	}

	if (m_b24H2OrGreater)
	{
		// 24H2 上,我们需要检查窗口次序,确保在桌面切换时候我们的窗口也显示在图标的正下方,
		// 而不被 Worker或者任何其他遮挡
		const int maxConsecutiveFixes = 5;
		int consecutiveFixCount = 0;
		HWND lastConflictHwnd = nullptr;
		HWND conflictHwnd = nullptr;
		HWND hEmbedWnd = (HWND)winId();
		//std::wcout << L"[Info] Start monitoring Z-order between embed window and ShellDefView...\n";

		bool ok = _EnsureEmbedWindowBelow(m_hShellDefView, hEmbedWnd, conflictHwnd);
		if (!ok)
		{
			if (!conflictHwnd)
			{
				consecutiveFixCount = 0;
			}
			else if (conflictHwnd == lastConflictHwnd)
			{
				consecutiveFixCount++;
			}
			else
			{
				lastConflictHwnd = conflictHwnd;
				consecutiveFixCount = 1;
			}

			if (conflictHwnd)
			{
				//std::wcout << L"[Warning] Detected conflict hwnd: " << conflictHwnd
				//	<< L", consecutive fix count: " << consecutiveFixCount << L"\n";
			}

			if (consecutiveFixCount >= maxConsecutiveFixes)
			{
				//std::wcerr << L"[Error] Detected repeated Z-order conflict! Exiting monitor.\n";
				MessageBoxW(NULL,
					L"可能存在竞争的窗口,出于保护,自动退出上升沉浸式桌面!",
					L"上升沉浸式桌面错误",
					MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL | MB_TOPMOST);
				PostMessageW(hEmbedWnd, WM_CLOSE, 0, 0);
				Sleep(1000);
				PostMessageW(hEmbedWnd, WM_DESTROY, 0, 0);
			}
		}
		else
		{
			// 没冲突，重置
			consecutiveFixCount = 0;
			lastConflictHwnd = nullptr;
		}
	}
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
	if (GM_ENGINE.IsWallpaper())
	{
		CComPtr<IDesktopWallpaper> pDesktopWallpaper;
		HRESULT hr = CoCreateInstance(CLSID_DesktopWallpaper, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pDesktopWallpaper));
		if (SUCCEEDED(hr))
		{
			// 获取主显示器ID
			LPWSTR monitorId = nullptr;
			hr = pDesktopWallpaper->GetMonitorDevicePathAt(0, &monitorId);
			if (SUCCEEDED(hr) && monitorId)
			{
				// 获取当前主显示器的壁纸路径
				LPWSTR wallpaperPath = nullptr;
				hr = pDesktopWallpaper->GetWallpaper(monitorId, &wallpaperPath);
				if (SUCCEEDED(hr) && wallpaperPath)
				{
					// 重新设置壁纸为当前壁纸
					pDesktopWallpaper->SetWallpaper(NULL, wallpaperPath);
					CoTaskMemFree(wallpaperPath);
				}
				CoTaskMemFree(monitorId);
			}
		}

		m_pTrayIcon->hide();
		delete m_pTrayIcon;
	}
	exit(0);
}

void CGMMainWindow::_slotSetAudioTime(int iTimeRatio) const
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

void CGMMainWindow::_slotWallpaperPlayOrPause()
{
	if (m_bPlayingMusic)
	{
		m_pWallpaperPlayAct->setText(QString::fromLocal8Bit("播放音乐"));
		m_bPlayingMusic = false;
		GM_ENGINE.Pause();
	}
	else
	{
		m_pWallpaperPlayAct->setText(QString::fromLocal8Bit("暂停音乐"));
		m_bPlayingMusic = true;
		GM_ENGINE.Play();
	}
}

bool CGMMainWindow::_IsOtherAppFullscreen()
{
	HWND fgWindow = GetForegroundWindow();
	if (fgWindow == (HWND)winId())return false;

	bool bHasFull = false;
	if (fgWindow != NULL)
	{
		bool bFull = _IsFullscreen(fgWindow);
		if (bFull)
		{
			TCHAR szClassName[256]; // 缓冲区
			int len = GetClassName(fgWindow, szClassName, _countof(szClassName));
			// 排除WorkerW窗口
			bool bNotWorkerW = (len > 0 && _tcsstr(szClassName, _T("WorkerW")) == nullptr);
			// 排除progman窗口
			bool bNotProgman = (len > 0 && _tcsstr(szClassName, _T("Progman")) == nullptr);
			// 排除XamlExplorerHostIslandWindow
			bool bNotXamlExplorer = (len > 0 && _tcsstr(szClassName, _T("XamlExplorerHostIslandWindow")) == nullptr);
			if (bNotWorkerW && bNotProgman)
			{
				//查找m_vFullWnds中是否已经存在该窗口
				bool bExist = false;
				for (auto& itr : m_vFullWnds)
				{
					if (itr == fgWindow)
					{
						bExist = true;
						break;
					}
				}
				bHasFull = true;
				if (!bExist) m_vFullWnds.push_back(fgWindow);
			}
		}
	}
	// 遍历m_vFullWnds，删除已经关闭的窗口
	auto itr = m_vFullWnds.begin();
	while (itr != m_vFullWnds.end())
	{
		if (!IsWindow(*itr))
		{
			itr = m_vFullWnds.erase(itr);
		}
		else
		{
			++itr;
		}
	}
	
	// 遍历m_vFullWnds，检查是否还有全屏窗口
	for (auto& itr : m_vFullWnds)
	{
		if (itr != fgWindow && _IsFullscreen(itr))
		{
			bHasFull = true;
			break;
		}
	}
	return bHasFull;
}

bool CGMMainWindow::_IsFullscreen(HWND hWindow) const
{
	if (hWindow == NULL) return false;

	RECT fgRect;
	GetWindowRect(hWindow, &fgRect);

	RECT workArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
	int screenWidth = workArea.right - workArea.left;
	int screenHeight = workArea.bottom - workArea.top;

	return (fgRect.right - fgRect.left >= screenWidth && fgRect.bottom - fgRect.top >= screenHeight);
}

//void CGMMainWindow::_OnTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
//{
//	if (reason == QSystemTrayIcon::Trigger) // 单击
//	{
//		if (m_pPlayKitWidget->isVisible())
//		{
//			m_pPlayKitWidget->hide();
//		}
//		else
//		{
//			m_pPlayKitWidget->raise();
//			m_pPlayKitWidget->show();
//		}
//	}
//}

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

bool CGMMainWindow::_RaiseDesktop(HWND hProgmanWnd)
{
	// See: https://github.com/valinet/ExplorerPatcher/issues/525
	if (hProgmanWnd)
	{
		DWORD_PTR res0 = -1, res1 = -1, res2 = -1, res3 = -1;
		// Call CDesktopBrowser::_IsDesktopWallpaperInitialized
		SendMessageTimeoutW(hProgmanWnd, 0x052C, 0xA, 0, SMTO_NORMAL, 1000, &res0);
		if (FAILED(res0))
		{
			return FALSE;
		}

		// Prepare to generate wallpaper window
		SendMessageTimeoutW(hProgmanWnd, 0x052C, 0xD, 0, SMTO_NORMAL, 1000, &res1);
		SendMessageTimeoutW(hProgmanWnd, 0x052C, 0XD, 1, SMTO_NORMAL, 1000, &res2);
		// "Animate desktop", which will make sure the wallpaper window is there
		SendMessageTimeoutW(hProgmanWnd, 0x052C, 0, 0, SMTO_NORMAL, 1000, &res3);  // 0 参数是必须的，对于早期系统(win7) 0xD 参数会导致处理失败。	
		return !res1 && !res2 && !res3;
	}
	return FALSE;
}

bool CGMMainWindow::_IsExplorerWorker(HWND hwnd)
{
	WCHAR className[256];
	GetClassNameW(hwnd, className, 256);
	if (wcscmp(className, L"WorkerW") != 0) {
		return false;
	}

	DWORD pid = 0;
	GetWindowThreadProcessId(hwnd, &pid);
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (!hProc) {
		return false;
	}

	WCHAR exeName[MAX_PATH];
	if (GetModuleBaseNameW(hProc, nullptr, exeName, MAX_PATH)) {
		_wcslwr_s(exeName, MAX_PATH);
		CloseHandle(hProc);
		return wcsstr(exeName, L"explorer.exe") != nullptr;
	}
	CloseHandle(hProc);
	return false;
}

bool CGMMainWindow::_EnsureEmbedWindowBelow(HWND hShellDefView, HWND hEmbedWnd, HWND& lpConflictWindow)
{
	HWND prev = GetWindow(hEmbedWnd, GW_HWNDPREV);
	if (prev == hShellDefView) {
		// 顺序已正确，无需操作
		lpConflictWindow = nullptr;
		return true;
	}

	//std::wcout << L"[Info] Fixing Z-order: moving embed window below ShellDefView\n";
	SetWindowPos(hEmbedWnd, hShellDefView, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	if (_IsExplorerWorker(prev))
	{
		lpConflictWindow = nullptr;
		return true;
	}

	lpConflictWindow = prev;
	return false;
}

void CGMMainWindow::_SetWallPaper(HWND hEmbedWnd)
{
	if (!hEmbedWnd) return;

	LONG_PTR exstyle_tw = GetWindowLongPtrW(hEmbedWnd, GWL_EXSTYLE);
	exstyle_tw |= WS_EX_LAYERED;
	exstyle_tw &= ~WS_EX_TOOLWINDOW;
	SetWindowLongPtrW(hEmbedWnd, GWL_EXSTYLE, exstyle_tw);

	LONG_PTR style_tw = GetWindowLongPtrW(hEmbedWnd, GWL_STYLE);
	style_tw &= ~(WS_CHILDWINDOW | WS_POPUP | WS_OVERLAPPED | WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_THICKFRAME);
	SetWindowLongPtrW(hEmbedWnd, GWL_STYLE, style_tw);

	// 24H2
	HWND hProgman = FindWindowW(L"Progman", L"Program Manager");

	if (!hProgman) return;
	if (!_RaiseDesktop(hProgman)) return;

	m_hShellDefView = FindWindowExW(hProgman, 0, L"SHELLDLL_DefView", L"");

	HWND hWorker1 = nullptr, hWorker2 = nullptr;
	if (!m_hShellDefView)  // 如果没有找到,则回退 23H2 的搜索模式
	{
		HWND hWorker_p1 = GetWindow(hProgman, GW_HWNDPREV);
		if (hWorker_p1)
		{
			m_hShellDefView = FindWindowExW(hWorker_p1, 0, L"SHELLDLL_DefView", L"");
			if (!m_hShellDefView)
			{
				hWorker2 = hWorker_p1;
				HWND hWorker_p2 = GetWindow(hWorker_p1, GW_HWNDPREV);
				if (hWorker_p2)
				{
					m_hShellDefView = FindWindowExW(hWorker_p2, 0, L"SHELLDLL_DefView", L"");
					if (m_hShellDefView)
					{
						hWorker1 = hWorker_p2;
					}
				}
			}
			else
			{
				hWorker1 = hWorker_p1;
			}
		}
	}

	HWND hWorker = FindWindowExW(hProgman, 0, L"WorkerW", L"");
	if (!hWorker) hWorker = FindWindowExW(hProgman, 0, L"WorkerA", L"");

	// 23H2
	if (!hWorker)
	{
		hWorker = !hWorker2 ? hProgman : hWorker2;
		m_b24H2OrGreater = false;
	}

	SetParent(hEmbedWnd, NULL);
	// 可以通过窗口透明化实现背景透明(务必作为顶级窗口,欺骗 DWM, 否则 DWM 将不允许透明窗口)
	// 有点像是 hack 生产环境不建议使用!
	//{
	//	MARGINS margins{ 0, 0, -1, -1 };
	//	DwmExtendFrameIntoClientArea(hEmbedWnd, &margins);

	//	DWM_BLURBEHIND bb = { 0 };
	//	HRGN hRgn = CreateRectRgn(0, 0, -1, -1); //应用毛玻璃的矩形范围，
	//	//参数(0,0,-1,-1)可以让整个窗口客户区变成透明的，而鼠标是可以捕获到透明的区域
	//	bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
	//	bb.hRgnBlur = hRgn;
	//	bb.fEnable = TRUE;
	//	DwmEnableBlurBehindWindow(hEmbedWnd, &bb);
	//}

	SetLayeredWindowAttributes(hEmbedWnd, 0, 0xFF, LWA_ALPHA);

	// 不建议设置窗口为 WorkerW 的子窗口,切换壁纸时候会被意外销毁!
	//if (m_b24H2OrGreater)
	//{
	//	SetWindowLongPtrW(hWorker, GWL_EXSTYLE, GetWindowLongPtrW(hWorker, GWL_EXSTYLE) | WS_EX_LAYERED);
	//	SetLayeredWindowAttributes(hWorker, RGB(0,0,0), 255, LWA_ALPHA | LWA_COLORKEY);
	//}

	SetParent(hEmbedWnd, m_b24H2OrGreater ? hProgman : hWorker);

	SetWindowPos(hEmbedWnd, HWND_TOP, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME);
	if (m_hShellDefView)
	{
		SetWindowPos(m_hShellDefView, HWND_TOP, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}
	SetWindowPos(hWorker, HWND_BOTTOM, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME);

	RECT rchTestOld{};
	GetWindowRect(hEmbedWnd, &rchTestOld);

	QList<QScreen*> mScreen = qApp->screens();
	// DPI 计算
	qreal dpr = mScreen[0]->devicePixelRatio();;
	int iLeft = mScreen[0]->geometry().topLeft().x() * dpr;
	int iTop = mScreen[0]->geometry().topLeft().y() * dpr;
	int iWidth	= mScreen[0]->geometry().width() * dpr;
	int iHeight	= mScreen[0]->geometry().height() * dpr;
	RECT rcFullScreen{ (LONG)iLeft, (LONG)iTop, (LONG)iWidth, (LONG)iHeight };

	AdjustWindowRect(&rcFullScreen, style_tw, FALSE);

	UINT rcfx = rcFullScreen.right - rcFullScreen.left;
	UINT rcfy = rcFullScreen.bottom - rcFullScreen.top;
	MoveWindow(hEmbedWnd, rcFullScreen.left, rcFullScreen.top, rcfx, rcfy, TRUE);

	WINDOWPLACEMENT wp{};
	wp.length = sizeof(WINDOWPLACEMENT);
	wp.flags = WPF_SETMINPOSITION;
	wp.showCmd = SW_SHOWNORMAL;
	wp.ptMinPosition = { rcFullScreen.left, rcFullScreen.top };
	wp.ptMaxPosition = { rcFullScreen.left, rcFullScreen.top };
	wp.rcNormalPosition = rcFullScreen;
	SetWindowPlacement(hEmbedWnd, &wp);

	ShowWindow(hProgman, SW_SHOW);
	ShowWindow(hEmbedWnd, SW_SHOW);
	ShowWindow(hWorker, SW_SHOW);

	if (m_b24H2OrGreater)
	{
		QList<QScreen*> mScreen = qApp->screens();
		m_pSceneWidget->setGeometry(
			mScreen[0]->geometry().topLeft().x(),
			mScreen[0]->geometry().topLeft().y(),
			mScreen[0]->geometry().width(),
			mScreen[0]->geometry().height());
	}
}

bool CGMMainWindow::_Is24H2OrGreater()
{
	typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
	HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
	if (!hMod) return false;

	RtlGetVersionPtr fn = (RtlGetVersionPtr)::GetProcAddress(hMod, "RtlGetVersion");
	if (!fn) return false;

	RTL_OSVERSIONINFOW rovi = { 0 };
	rovi.dwOSVersionInfoSize = sizeof(rovi);
	if (fn(&rovi) != 0) return false;

	// Windows 11 24H2: major=10, minor=0, build>=26100
	return (rovi.dwMajorVersion == 10 && rovi.dwMinorVersion == 0 && rovi.dwBuildNumber >= 26100);
}