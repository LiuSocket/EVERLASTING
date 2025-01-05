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

	/** @brief ��ʼ�� */
	bool Init();
	/** @brief ���� */
	void Update();

	/**
	* @brief ����ȫ���л�
	* @param bFull trueΪȫ����false Ϊ��ȫ��
	*/
	void SetFullScreen(const bool bFull);
	/**
	* @brief ��ȡ����ȫ��״̬
	* @return bool		�Ƿ���ȫ��״̬
	*/
	bool GetFullScreen();

public slots:

	/** @brief ��С��/��ԭ */
	void _slotMinimum();
	/** @brief ���/��ԭ */
	void _slotMaximum();
	/** @brief �ر� */
	void _slotClose();
	/** @brief ����ȫ�� */
	void _slotFullScreen();

protected:
	void changeEvent(QEvent* event);
	void resizeEvent(QResizeEvent* event);
	void closeEvent(QCloseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent* event);
	/** @brief �����ϵļ����¼� */
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

private:

private:
	Ui::GMMainWindow					ui;
	CGMViewWidget*						m_pSceneWidget;

	bool								m_bInit;
	bool								m_bFull;				//!< �Ƿ�ȫ��
	bool								m_bPressed;				//!< �Ƿ������
	QPoint								m_vPos;					//!< ���ڵ�λ��
};