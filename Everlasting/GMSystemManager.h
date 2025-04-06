//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMSystemManager.h
/// @brief		system Manager ϵͳ������
/// @version	1.0
/// @author		LiuTao
/// @date		2021.08.14
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <windows.h>
#include <QObject>
#include "../Engine/GMPrerequisites.h"
#include "../Engine/GMEngine.h"

/*************************************************************************
Macro Defines
*************************************************************************/
#define GM_SYSTEM_MANAGER					CGMSystemManager::getSingleton()
#define GM_SYSTEM_MANAGER_PTR				CGMSystemManager::getSingletonPtr()

/*************************************************************************
Enums
*************************************************************************/


/*************************************************************************
Struct
*************************************************************************/


/*************************************************************************
Class
*************************************************************************/
/*!
*  @class CGMSystemManager
*  @brief ϵͳ������
*/
class CGMSystemManager : public QObject, public GM::CGMSingleton<CGMSystemManager>
{
	Q_OBJECT

public:
	CGMSystemManager();
	~CGMSystemManager();

	/** @brief ��ʼ�� */
	bool Init();
	/** @brief �ͷ� */
	bool Release();

	/** @brief ���̰��������¼� */
	bool GMKeyDown(GM::EGMKeyCode eKC);
	/** @brief ���̰��������¼� */
	bool GMKeyUp(GM::EGMKeyCode eKC);

	/** @brief ���������ʾ/���� */
	void SetCursorVisible(bool bVisible);

protected:
	/** @brief ��ʱ������ */
	void timerEvent(QTimerEvent *event);

private:
	/** @brief ��Ⱦ���� */
	void _Render();

public:
	/** @brief ��ȡ���� */
	static CGMSystemManager& getSingleton(void);
	static CGMSystemManager* getSingletonPtr(void);

private:
	bool							m_bInit = false;				//!< ��ʼ����ʶ
	bool							m_bFirst = true;				//!< �Ƿ��һ֡

	uint							m_nKeyMask = 0;
};