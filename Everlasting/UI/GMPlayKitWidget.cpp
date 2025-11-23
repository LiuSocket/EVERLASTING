#include "GMPlayKitWidget.h"
#include "../Engine/GMEngine.h"
#include <QKeyEvent>
#include <iostream>

#include <WtsApi32.h>	// WTSRegisterSessionNotification
#pragma comment(lib, "Wtsapi32.lib")

using namespace GM;

CGMPlayKitWidget::CGMPlayKitWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	setAttribute(Qt::WA_TranslucentBackground);

	connect(ui.playBtn, SIGNAL(clicked()), this, SLOT(_slotPlayOrPause()));

#if defined(Q_OS_WIN) || defined(_WIN32)
	// 注册指定窗口以接收会话更改通知，获取锁屏，解锁，登录，注销等消息
	bool ret = WTSRegisterSessionNotification((HWND)winId(), NOTIFY_FOR_THIS_SESSION);
	//std::cout << "会话事件通知注册" << (ret ? "成功" : "失败") << std::endl;
	// 注册以便在系统暂停或恢复时接收通知，最低支持 win8
	(HPOWERNOTIFY)m_hPowerNotify = RegisterSuspendResumeNotification((HWND)winId(), DEVICE_NOTIFY_WINDOW_HANDLE);
	//std::cout << "电源事件通知注册" << (m_hPowerNotify != NULL ? "成功" : "失败") << std::endl;
	if (ret && m_hPowerNotify)
	{
		m_bWtsRegistered = true;
	}
#endif
}

CGMPlayKitWidget::~CGMPlayKitWidget()
{
#if defined(_WIN32) || defined(Q_OS_WIN)
	// 反注册会话通知
	if (m_bWtsRegistered)
	{
		WTSUnRegisterSessionNotification((HWND)winId());
		m_bWtsRegistered = false;
	}
	// 反注册挂起/恢复通知
	if (m_hPowerNotify)
	{
		UnregisterSuspendResumeNotification(reinterpret_cast<HPOWERNOTIFY>(m_hPowerNotify));
		m_hPowerNotify = nullptr;
	}
#endif
}

void CGMPlayKitWidget::Update()
{
	const std::wstring wstrAudioName = GM_ENGINE.GetAudioName();
	QString strFileName = QString::fromStdWString(wstrAudioName);
	if ("" == strFileName || GM_ENGINE.IsAudioOver())
	{
		// 将播放/暂停按钮设置成暂停状态
		ui.playBtn->setChecked(false);
		return;
	}
}

bool CGMPlayKitWidget::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
	// 仅在 Windows 平台 Qt 常见的两种标识
	if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher")
	{
		MSG* msg = reinterpret_cast<MSG*>(message);
		if (msg)
		{
			if (WM_POWERBROADCAST == msg->message)
			{
				WPARAM wp = msg->wParam;
				// 唤醒
				if (wp == PBT_APMRESUMEAUTOMATIC || wp == PBT_APMRESUMECRITICAL)
				{
					emit signalWakeUpWallpaper();
					//std::cout << "[Info] 唤醒" << std::endl;
				}
			}
			else if (WM_WTSSESSION_CHANGE == msg->message)
			{
				WPARAM wp = msg->wParam;
				// 解锁、登录
				if (wp == WTS_SESSION_UNLOCK || wp == WTS_SESSION_LOGON)
				{
					emit signalWakeUpWallpaper();
					//std::cout << "[Info] 解锁" << std::endl;
				}
			}
			else{}
		}
	}

	return QWidget::nativeEvent(eventType, message, result);
}

void CGMPlayKitWidget::_slotPlayOrPause()
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