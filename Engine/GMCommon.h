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
	constexpr float PROGRESS_0 = 0.005f; 		// ǰ̫��ʱ������������ɲ��ʱ��
	constexpr float PROGRESS_1 = 0.030f; 		// ɲ��ʱ����������ʼ����������̬����ת������
	constexpr float PROGRESS_1_1 = 0.04f; 		// ɲ��ʱ�������ġ����򻷡�����ת�����ʧ
	constexpr float PROGRESS_2 = 0.090f;		// ������̬��������
	constexpr float PROGRESS_2_1 = 0.1f;		// ������ٵ�����β����ʧ
	constexpr float PROGRESS_3 = 0.150f;		// ������̬����������������������з����෴
	constexpr float PROGRESS_3_1 = 0.152f;		// ȫ�򷢶��������ɹ�������
	constexpr float PROGRESS_4 = 0.183f;		// ������ȫ���ʿ���

	/*************************************************************************
	 Macro Defines
	*************************************************************************/

	#define GM_UNIT_SCALE				(1e5)		// ÿ���㼶�ռ�֮��ĳ߶ȱ���
	#define GM_HIERARCHY_MAX			(6)			// �������㼶
	#define GM_MIN_RADIUS				(5e-4)		// �����ÿ���㼶�ռ��е���С����
	#define GM_MAX_RADIUS				(GM_MIN_RADIUS*GM_UNIT_SCALE*2)	// �����ÿ���㼶�ռ��е���󽹾�
	#define GM_HANDLE_RADIUS			(0.15)		// ���ְ뾶����λ������꣬1e20

	#define TRANS_ALT_NUM				(512)			// ͸����ͼ�ĸ߶Ȳ����� [0,fAtmosThick]m
	#define TRANS_PITCH_NUM				(512)			// ͸����ͼ��̫������������ֵ������ [��ƽ������ֵ,1]

	#define IRRA_ALT_NUM				(128)			// ���նȵĸ߶Ȳ����� [0,fAtmosThick]m
	#define IRRA_UP_NUM					(256)			// ���նȵ�̫���������Ϸ���ĵ�˲����� [-1,1]

	#define SCAT_PITCH_NUM				(128)		// ɢ��ͼ�ĸ����������Ϸ���ĵ�˲����� [-1,1]
	#define SCAT_LIGHT_NUM				(32)		// ɢ��ͼ��̫���������Ϸ���ĵ�˲����� [-1,1]
	#define SCAT_COS_NUM				(8)			// ɢ��ͼ��̫�����������߷���ĵ�˲����� [-1,1]
	#define SCAT_ALT_NUM				(32)		// ɢ��ͼ�Ĵ�����߶Ȳ����� [0,fAtmosThick]m

	/*************************************************************************
	 Enums
	*************************************************************************/

	/*!
	 *  @enum EGMAtmosHeight
	 *  @brief �������ö�٣�Ϊ������㣬��������������ɼ����޶�ֵ
	 */
	enum EGMAtmosHeight
	{
		EGMAH_0,		//!< û�д���
		EGMAH_15,		//!< 15km���
		EGMAH_30,		//!< 30km���
		EGMAH_60,		//!< 60km���
		EGMAH_120,		//!< 120km���
	};

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
		int								iScreenWidth;			//!< ��Ļ���ȣ���λ������
		int								iScreenHeight;			//!< ��Ļ�߶ȣ���λ������
	};
}	// GM