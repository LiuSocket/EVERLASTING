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
	bool GetFullScreen() const;

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

private slots:

	/** @brief 初始化GM_Viewer */
	void _slotInitGMViewer();

private:
	Ui::GMMainWindow					ui;
	CGMViewWidget*						m_pSceneWidget = nullptr;

	bool								m_bInit = false;
	bool								m_bFull = false;			//!< 是否全屏
	bool								m_bPressed = false;			//!< 是否按下鼠标
	QPoint								m_vPos;						//!< 窗口的位置
};