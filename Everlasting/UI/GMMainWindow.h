#pragma once

#include <QMainWindow>
#include "ui_GMMainWindow.h"

/*************************************************************************
Class
*************************************************************************/
class CGMViewWidget;

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
	bool GetFullScreen();

public slots:

	/** @brief 最小化/还原 */
	void _slotMinimum();
	/** @brief 最大化/还原 */
	void _slotMaximum();
	/** @brief 关闭 */
	void _slotClose();
	/** @brief 启用全屏 */
	void _slotFullScreen();

protected:
	void changeEvent(QEvent* event);
	void resizeEvent(QResizeEvent* event);
	void closeEvent(QCloseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent* event);
	/** @brief 界面上的键盘事件 */
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

private:

private:
	Ui::GMMainWindow					ui;
	CGMViewWidget*						m_pSceneWidget;

	bool								m_bInit;
	bool								m_bFull;				//!< 是否全屏
	bool								m_bPressed;				//!< 是否按下鼠标
	bool								m_bShowVolume;			//!< 是否显示实时变化的音量
	QPoint								m_vPos;					//!< 窗口的位置
	int									m_iAudioDuration;		//!< 音频总时长，单位：ms
	QString								m_strName;				//!< 音频文件名称，包含后缀名
};