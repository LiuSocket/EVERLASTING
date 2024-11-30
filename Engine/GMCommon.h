//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMStructs.h
/// @brief		Galaxy-Music Engine - Common
/// @version	1.0
/// @author		LiuTao
/// @date		2020.12.09
//////////////////////////////////////////////////////////////////////////
#pragma once
#include <Windows.h>
#include "GMStructs.h"
#include "GMEnums.h"

namespace GM
{
	/*************************************************************************
	constexpr
	*************************************************************************/

	/*************************************************************************
	 Macro Defines
	*************************************************************************/
	#define GM_MAIN_MASK					(0x1)			// ���������
	#define GM_SHADOW_CAST_MASK				(0x1 << 7)		// Ͷ����Ӱ����

	#define SHADOW_TEX_UNIT					15				// ��Ӱ����Ԫ

	/*************************************************************************
	 Enums
	*************************************************************************/

	/*************************************************************************
	 Class
	*************************************************************************/

	/*************************************************************************
	 Struct
	*************************************************************************/
	/*!
	 *  @struct SGMConfigData
	 *  @brief ��������
	 */
	struct SGMConfigData
	{
		SGMConfigData()
			: strCorePath("../../Data/Core/"), strMediaPath(L"../../Data/Media/"),
			eRenderQuality(EGMRENDER_LOW), fFovy(40.0f), iScreenWidth(1920), iScreenHeight(1080)
		{}

		std::string						strCorePath;			//!< ������Դ·��
		std::wstring					strMediaPath;			//!< �ⲿ��Դ·��
		EGMRENDER_QUALITY				eRenderQuality;			//!< �߻���ģʽ
		float							fFovy;					//!< ����Ĵ�ֱFOV����λ����
		int								iScreenWidth;			//!< ��Ļ��ȣ���λ������
		int								iScreenHeight;			//!< ��Ļ�߶ȣ���λ������
	};
}	// GM
