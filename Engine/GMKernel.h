//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMStructs.h
/// @brief		Galaxy-Music Engine -Kernel
/// @version	1.0
/// @author		LiuTao
/// @date		2020.12.09
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <osg/Group>
#include <osgViewer/Viewer>
#include "GMViewWidget.h"

namespace GM
{ 
	/*************************************************************************
	 Macro Defines
	*************************************************************************/
	#define GM_Root					m_pKernelData->vRoot
    #define GM_ViewWidget			m_pKernelData->pViewWidget
	#define GM_Viewer				GM_ViewWidget->getOsgViewer()

	/*************************************************************************
	 Struct
	*************************************************************************/
	/*!
	 *  @struct SGMKernelData
	 *  @brief 内核数据
	 */
	struct SGMKernelData
	{
		bool									bInited = false;		//!< 是否初始化
		osg::ref_ptr<osg::Group>				vRoot;					//!< 根节点
		CGMViewWidget*							pViewWidget = nullptr;	//!< 继承了osgQOpenGLWidget
		osg::ref_ptr<osg::Camera>				pBackgroundCam;			//!< 背景RTT相机
		osg::ref_ptr<osg::Camera>				pForegroundCam;			//!< 前景RTT相机
	};

	/*!
	*  @brief 渲染顺序
	*/


}	// GM
