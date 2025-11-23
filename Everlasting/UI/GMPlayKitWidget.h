#pragma once

#include <QWidget>
#include "ui_GMPlayKitWidget.h"

class CGMPlayKitWidget : public QWidget
{
	Q_OBJECT

public:
	CGMPlayKitWidget(QWidget *parent = nullptr);
	~CGMPlayKitWidget();

	/** @brief 更新 */
	void Update();

protected:
	// 在 Windows 原生消息中捕捉唤醒（休眠恢复）事件
	bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

signals:
	void signalWakeUpWallpaper();	//!< 唤醒桌面壁纸信号

private slots:
	/** @brief 播放/暂停 */
	void _slotPlayOrPause();

private:
	Ui::playKitWidget ui;

#if defined(Q_OS_WIN) || defined(_WIN32)
	void*				m_hPowerNotify = nullptr;		//!< RegisterSuspendResumeNotification 返回值（存为 void*）
	bool				m_bWtsRegistered = false;		//!< WTS 注册成功标记
#endif
};
