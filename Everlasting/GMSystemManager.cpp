//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMSystemManager.cpp
/// @brief		system Manager 系统管理器
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

#define FRAME_UPDATE	15 	// 多少帧更新一次信息

/*************************************************************************
CGMSystemManager Methods
*************************************************************************/

CGMSystemManager::CGMSystemManager()
{
}

CGMSystemManager::~CGMSystemManager()
{

}

CGMSystemManager* CGMSingleton<CGMSystemManager>::msSingleton = nullptr;

/** @brief 获取单例 */
CGMSystemManager& CGMSystemManager::getSingleton(void)
{
	if (!msSingleton)
		msSingleton = GM_NEW(CGMSystemManager);
	assert(msSingleton);
	return (*msSingleton);
}

/** @brief 初始化 */
bool CGMSystemManager::Init()
{
	if (m_bInit)
		return true;

	// 初始化引擎
	GM_ENGINE.Init();
	// 初始化界面
	GM_UI_MANAGER.Init();

	// 启动定时器
	startTimer(30);

	m_bInit = true;
	return true;
}

/** @brief 释放 */
bool CGMSystemManager::Release()
{
	if (!m_bInit)
		return true;

	GM_UI_MANAGER.Release();
	GM_ENGINE.Release();

	m_bInit = false;
	return true;
}

void CGMSystemManager::SetCursorVisible(bool bVisible)
{
	GM_UI_MANAGER.SetCursorVisible(bVisible);
}

/** @brief 定时器更新 */
void CGMSystemManager::timerEvent(QTimerEvent *event)
{
	if (m_bFirst)
	{
		GM_ENGINE.Welcome();
		m_bFirst = false;
	}

	// 间隔更新
	m_iFrameCount++;
	if (FRAME_UPDATE <= m_iFrameCount)
	{
		if (GM_ENGINE.IsWelcomeFinished())
		{
			GM_UI_MANAGER.UpdateAudioInfo();
		}
		if (GM_ENGINE.IsWallpaper())
		{
			GM_UI_MANAGER.UpdateWallpaper();
		}

		m_iFrameCount = 0;
	}

	_Render();
}

void CGMSystemManager::_Render()
{
	GM_ENGINE.Update();
	GM_UI_MANAGER.Update();
}