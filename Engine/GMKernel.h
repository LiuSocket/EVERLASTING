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
#include "GMViewWidget.h"

namespace GM
{ 
	/*************************************************************************
	 Macro Defines
	*************************************************************************/
	#define GM_Root					m_pKernelData->vRoot
	#define GM_View					m_pKernelData->vView
	#define GM_Viewer				m_pKernelData->vViewer

	/*************************************************************************
	 Struct
	*************************************************************************/
	/*!
	 *  @struct SGMKernelData
	 *  @brief 内核数据
	 */
	struct SGMKernelData
	{
		bool										bInited = false;	//!< 是否初始化
		osg::ref_ptr<osg::Group>					vRoot;				//!< 根节点
		osg::ref_ptr<osgViewer::View>				vView;				//!< 视口
		osg::ref_ptr<CGMViewWidget>					vViewer;			//!< 视口管理器
		osg::ref_ptr<osg::Camera>					pBackgroundCam;		//!< 背景RTT相机
		osg::ref_ptr<osg::Camera>					pForegroundCam;		//!< 前景RTT相机
	};

}	// GM
