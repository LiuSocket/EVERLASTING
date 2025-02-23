//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMCharacter.h
/// @brief		GMEngine - Character Module
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.07
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommon.h"
#include "GMKernel.h"

#include <vector>
#include <random>

namespace GM
{
	/*************************************************************************
	 Enums
	*************************************************************************/
	// ����������ö��ֵ
	enum EGMANIMATION_BONE
	{
		/** idle */
		EA_BONE_IDLE,
		/** ͷ���� */
		EA_BONE_HEAD_L,
		/** ͷ���ҿ� */
		EA_BONE_HEAD_R,
		/** ͷ���Ͽ� */
		EA_BONE_HEAD_U,
		/** ͷ���¿� */
		EA_BONE_HEAD_D
	};
	// ���ζ�����ö��ֵ
	enum EGMANIMATION_MORPH
	{
		/** գ�� */
		EA_MORPH_BLINK,
		/** ���͡����� */
		EA_MORPH_AA,
		/** ���͡�Ŷ�� */
		EA_MORPH_OO
	};
	/*************************************************************************
	 Structs
	*************************************************************************/

	/*************************************************************************
	 Class
	*************************************************************************/

	/*!
	 *  @class CGMCharacter��ϵͳ��λ������
	 *  @brief GM Character Module
	 */
	class CGMCharacter
	{
	// ����
	public:
		/** @brief ���� */
		CGMCharacter();
		/** @brief ���� */
		~CGMCharacter();

		/** @brief ��ʼ�� */
		bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData);
		/** @brief ���� */
		bool Update(double dDeltaTime);
		/** @brief ����(�������������̬֮��) */
		bool UpdatePost(double dDeltaTime);

		/**
		* @brief �����ɫ�Ķ������ܣ��������������ζ�����
		* @param strName: ��ɫ�ڳ����е�����
		* @param pNode: ��ɫ�Ľڵ�ָ��
		* @return bool �ɹ�OK��ʧ��Fail����ɫ�����ڷ���NotExist
		*/
		bool InitAnimation(const std::string& strName, osg::Node* pNode);

	private:
		void _InnerUpdate(const double dDeltaTime);
		/** @brief ��ʱ����գ��״̬ */
		void _InnerUpdateBlink(const double dDeltaTime);
		/** @brief �ı�ע�ӷ��� */
		void _ChangeLookDir(const double dDeltaTime);

		/** @brief ÿ֡����תͷ�����Ĺ���״̬ */
		void _UpdateLookDir(const double dDeltaTime);

		/**
		* @brief ƽ�����ɺ���
		* @param fMin, fMax:			��Χ
		* @param fX:					ӳ��ǰ��ֵ
		* @return double:				ӳ����ֵ[0.0, 1.0]
		*/
		inline double _Smoothstep(const double fMin, const double fMax, const double fX)
		{
			double y = osg::clampBetween((fX - fMin) / (fMax - fMin), 0.0, 1.0);
			return y * y * (3 - 2 * y);
		}

	// ����
	private:
		SGMKernelData* m_pKernelData = nullptr;					//!< �ں�����
		SGMConfigData* m_pConfigData = nullptr;					//!< ��������

		std::default_random_engine m_iRandom;					//!< ���ֵ
		std::uniform_int_distribution<> m_iPseudoNoise;			//!< α������ֲ�
		std::uniform_int_distribution<> m_iRandomAngle;			//!< ����Ƕȷֲ���[0,360]

		std::string m_strName = "";								//!< ��ɫ����
		float m_fLookDuration = 10.0;							//!< ע�ӳ���ʱ�䣬��λ����
		float m_fTurnDuration = 1.0f;							//!< תͷ��ʱ����λ����
		float m_fMixTime = 1.0f;								//!< ��ǰ�����������ʱ�䣬��λ����

		float m_fHeadingTargetWeight = 0;						//!< ת�򶯻���Ŀ��Ȩ��
		float m_fPitchTargetWeight = 0;							//!< ����������Ŀ��Ȩ��
		float m_fHeadingSourceWeight = 0;						//!< ת�򶯻�����ʼȨ��
		float m_fPitchSourceWeight = 0;							//!< ������������ʼȨ��
		osg::Vec2 m_vHeadingWeight = osg::Vec2(0, 0);			//!< 2��ת�򶯻���Ȩ��, x = left, y = right
		osg::Vec2 m_vPitchWeight = osg::Vec2(0, 0);				//!< 2������������Ȩ��, x= up, y = down

		EGMANIMATION_BONE m_eHeadingAnim = EA_BONE_HEAD_L;		//!< ת�򶯻�
		EGMANIMATION_BONE m_eNextHeadingAnim = EA_BONE_HEAD_L;	//!< ��һ��ת�򶯻�
		EGMANIMATION_BONE m_ePitchAnim = EA_BONE_HEAD_U;			//!< ��������
		EGMANIMATION_BONE m_eNextPitchAnim = EA_BONE_HEAD_U;		//!< ��һ����������

		std::vector<std::string> m_strBoneAnimNameVec;			//!< ������������vector
		std::vector<std::string> m_strMorphAnimNameVec;			//!< ���ζ�������vector
	};
}	// GM
