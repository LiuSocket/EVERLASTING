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
	constexpr float PROGRESS_0 = 0.005f; 		// 前太阳时代结束，开启刹车时代
	constexpr float PROGRESS_1 = 0.030f; 		// 刹车时代结束，开始调整地球姿态，旋转北极轴
	constexpr float PROGRESS_1_1 = 0.04f; 		// 刹车时代残留的“地球环”由于转向而消失
	constexpr float PROGRESS_2 = 0.090f;		// 地球姿态调整中期
	constexpr float PROGRESS_2_1 = 0.1f;		// 地球加速的螺旋尾迹消失
	constexpr float PROGRESS_3 = 0.150f;		// 地球姿态调整结束，北极与地球运行方向相反
	constexpr float PROGRESS_3_1 = 0.152f;		// 全球发动机并网成功，启航
	constexpr float PROGRESS_4 = 0.183f;		// 发动机全功率开启

	/*************************************************************************
	 Macro Defines
	*************************************************************************/

	#define GM_UNIT_SCALE				(1e5)		// 每个层级空间之间的尺度比例
	#define GM_HIERARCHY_MAX			(6)			// 宇宙最大层级
	#define GM_MIN_RADIUS				(5e-4)		// 相机在每个层级空间中的最小焦距
	#define GM_MAX_RADIUS				(GM_MIN_RADIUS*GM_UNIT_SCALE*2)	// 相机在每个层级空间中的最大焦距
	#define GM_HANDLE_RADIUS			(0.15)		// 把手半径，单位：万光年，1e20

	#define TRANS_ALT_NUM				(512)			// 透过率图的高度采样数 [0,fAtmosThick]m
	#define TRANS_PITCH_NUM				(512)			// 透过率图的太阳俯仰角余弦值采样数 [地平线余弦值,1]

	#define IRRA_ALT_NUM				(128)			// 辐照度的高度采样数 [0,fAtmosThick]m
	#define IRRA_UP_NUM					(256)			// 辐照度的太阳方向与上方向的点乘采样数 [-1,1]

	#define SCAT_PITCH_NUM				(128)		// 散射图的俯仰方向与上方向的点乘采样数 [-1,1]
	#define SCAT_LIGHT_NUM				(32)		// 散射图的太阳方向与上方向的点乘采样数 [-1,1]
	#define SCAT_COS_NUM				(8)			// 散射图的太阳方向与视线方向的点乘采样数 [-1,1]
	#define SCAT_ALT_NUM				(32)		// 散射图的大气点高度采样数 [0,fAtmosThick]m

	/*************************************************************************
	 Enums
	*************************************************************************/

	/*!
	 *  @enum EGMAtmosHeight
	 *  @brief 大气厚度枚举，为方便计算，将大气厚度量化成几个限定值
	 */
	enum EGMAtmosHeight
	{
		EGMAH_0,		//!< 没有大气
		EGMAH_15,		//!< 15km厚度
		EGMAH_30,		//!< 30km厚度
		EGMAH_60,		//!< 60km厚度
		EGMAH_120,		//!< 120km厚度
	};

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
