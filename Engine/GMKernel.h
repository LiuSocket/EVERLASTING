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
	 *  @brief �ں�����
	 */
	struct SGMKernelData
	{
		bool										bInited = false;	//!< �Ƿ��ʼ��
		osg::ref_ptr<osg::Group>					vRoot;				//!< ���ڵ�
		osg::ref_ptr<osgViewer::View>				vView;				//!< �ӿ�
		osg::ref_ptr<CGMViewWidget>					vViewer;			//!< �ӿڹ�����
		osg::ref_ptr<osg::Camera>					pBackgroundCam;		//!< ����RTT���
		osg::ref_ptr<osg::Camera>					pForegroundCam;		//!< ǰ��RTT���
	};

}	// GM
