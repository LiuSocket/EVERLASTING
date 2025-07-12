#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>
#include "ui_GMMainWindow.h"

/*************************************************************************
Class
*************************************************************************/
class CGMVolumeWidget;
class CGMViewWidget;
class CGMPlayKitWidget;

class CGMMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	CGMMainWindow(QWidget *parent = Q_NULLPTR);
	~CGMMainWindow();

	/** @brief 初始化 */
	bool Init();
	/** @brief 更新 */
	void Update();

	/**
	* @brief 界面全屏切换
	* @param bFull true为全屏，false 为非全屏
	*/
	void SetFullScreen(const bool bFull);
	/**
	* @brief 获取界面全屏状态
	* @return bool		是否处于全屏状态
	*/
	inline bool CGMMainWindow::GetFullScreen() const{ return m_bFull; }

	/**
	* @brief 更新音频的所有信息
	*/
	void UpdateAudioInfo();

	/**
	* @brief 设置是否显示实时变化的音量
	* @param bVisible 是否显示实时变化的音量
	*/
	void SetVolumeVisible(const bool bVisible);

private slots:

	/** @brief 上一首 */
	void _slotLast();

	/** @brief 播放/暂停 */
	void _slotPlayOrPause();

	/** @brief 下一首 */
	void _slotNext();

	/** @brief 最小化/还原 */
	void _slotMinimum();
	/** @brief 最大化/还原 */
	void _slotMaximum();
	/** @brief 关闭 */
	void _slotClose();

	/** 
	* @brief 设置音频当前的时间
	* @param iTimeRatio: 当前时间与总时间的比值,[0,100]
	*/
	void _slotSetAudioTime(int iTimeRatio);

	/** @brief 开启声音/静音 */
	void _slotSetMute();
	/**
	* @brief 设置音量
	* @param iVolume	音量，[0, 100]
	*/
	void _slotSetVolume(int iVolume);

	/**
	* @brief 启用全屏
	*/
	void _slotFullScreen();

	/**
	* @brief 系统托盘图标被激活
	*/
	void _OnTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

protected:
	void changeEvent(QEvent* event);
	void resizeEvent(QResizeEvent* event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent* event);
	bool eventFilter(QObject* obj, QEvent* event) override;

private:
	/**
	* @brief 毫秒数 转 分钟:秒数
	* @param ms: 输入毫秒数
	* @param minutes: 输出分钟数
	* @param seconds: 输出秒数
	*/
	void _Million2MinutesSeconds(const int ms, int& minutes, int& seconds);

	/**
	* @brief 将hPlayer变成桌面窗口，构建号从26100开始，桌面窗口规则大变，需要单独处理
	* @param hPlayer: 我们程序的窗口句柄
	* @return bool 是否成功
	*/
	bool _SetWallPaper(HWND hPlayer);

private:
	Ui::GMMainWindow					ui;
	CGMVolumeWidget*					m_pVolumeWidget = nullptr;
	CGMViewWidget*						m_pSceneWidget = nullptr;
	CGMPlayKitWidget*					m_pPlayKitWidget = nullptr;		//!< 播放器工具控件

	bool								m_bInit = false;
	bool								m_bFull = false;					//!< 是否全屏
	bool								m_bPressed = false;				//!< 是否按下鼠标
	bool								m_bShowVolume = false;			//!< 是否显示实时变化的音量
	QPoint								m_vPos = QPoint(0, 0);			//!< 窗口的位置
	int									m_iAudioDuration = 5000;		//!< 音频总时长，单位：ms
	QString								m_strName = QString();			//!< 音频文件名称，包含后缀名
};