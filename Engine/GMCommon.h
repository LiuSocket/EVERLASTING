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
	#define GM_MAIN_MASK					(0x1)			// 主相机掩码
	#define GM_SHADOW_CAST_MASK				(0x1 << 7)		// 投射阴影掩码

	#define SHADOW_TEX_UNIT					15				// 阴影纹理单元

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
	 *  @brief 配置数据
	 */
	struct SGMConfigData
	{
		SGMConfigData()
			: strCorePath("../../Data/Core/"), strMediaPath(L"../../Data/Media/"),
			eRenderQuality(EGMRENDER_LOW), fFovy(40.0f), iScreenWidth(1920), iScreenHeight(1080)
		{}

		std::string						strCorePath;			//!< 核心资源路径
		std::wstring					strMediaPath;			//!< 外部资源路径
		EGMRENDER_QUALITY				eRenderQuality;			//!< 高画质模式
		float							fFovy;					//!< 相机的垂直FOV，单位：°
		int								iScreenWidth;			//!< 屏幕宽度，单位：像素
		int								iScreenHeight;			//!< 屏幕高度，单位：像素
	};
}	// GM
