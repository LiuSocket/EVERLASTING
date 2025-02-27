//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMSystemManager.cpp
/// @brief		system Manager ϵͳ������
/// @version	1.0
/// @author		LiuTao
/// @date		2021.08.14
//////////////////////////////////////////////////////////////////////////

#include "GMSystemManager.h"
#include "UI/GMUIManager.h"

#include <thread>
#include <QDesktopWidget>
#include <QApplication>
#include <QSettings>
#include <QTextCodec>
#include <QDatetime.h>
#include <QKeyEvent>

using namespace GM;

/*************************************************************************
Macro Defines
*************************************************************************/

/*************************************************************************
CGMSystemManager Methods
*************************************************************************/

CGMSystemManager::CGMSystemManager()
	: m_bInit(false), m_bFirst(true), m_nKeyMask(0)
{
}

CGMSystemManager::~CGMSystemManager()
{

}

CGMSystemManager* CGMSingleton<CGMSystemManager>::msSingleton = nullptr;

/** @brief ��ȡ���� */
CGMSystemManager& CGMSystemManager::getSingleton(void)
{
	if (!msSingleton)
		msSingleton = GM_NEW(CGMSystemManager);
	assert(msSingleton);
	return (*msSingleton);
}

CGMSystemManager* CGMSystemManager::getSingletonPtr(void)
{
	if (!msSingleton)
		msSingleton = GM_NEW(CGMSystemManager);
	assert(msSingleton);
	return msSingleton;
}

/** @brief ��ʼ�� */
bool CGMSystemManager::Init()
{
	if (m_bInit)
		return true;

	// ��ʼ������
	GM_ENGINE.Init();

	// ��ʼ������
	GM_UI_MANAGER.Init();

	// ������ʱ��
	startTimer(16);

	m_bInit = true;
	return true;
}

/** @brief �ͷ� */
bool CGMSystemManager::Release()
{
	if (!m_bInit)
		return true;

	GM_ENGINE.Release();
	m_bInit = false;
	return true;
}

/** @brief ���̰��������¼� */
bool CGMSystemManager::GMKeyDown(EGMKeyCode eKC)
{
	switch (eKC)
	{
	case EGMKeyCode::EGM_KC_F11:
	{
		GM_UI_MANAGER.SetFullScreen(!GM_UI_MANAGER.GetFullScreen());
	}
	break;
	case EGMKeyCode::EGM_KC_Escape:
	{
		GM_UI_MANAGER.SetFullScreen(false);
	}
	break;
	}
	return true;
}

/** @brief ���̰��������¼� */
bool CGMSystemManager::GMKeyUp(EGMKeyCode eKC)
{
	switch (eKC)
	{
	case EGMKeyCode::EGM_KC_Up:
	case EGMKeyCode::EGM_KC_Down:
	{
	}
	break;
	}
	return true;
}

void CGMSystemManager::SetCursorVisible(bool bVisible)
{
	GM_UI_MANAGER.SetCursorVisible(bVisible);
}

/** @brief ��ʱ������ */
void CGMSystemManager::timerEvent(QTimerEvent *event)
{
	if (m_bFirst)
	{
		m_bFirst = false;
	}

	_Render();
}

void CGMSystemManager::_Render()
{
	GM_ENGINE.Update();
	GM_UI_MANAGER.Update();
}