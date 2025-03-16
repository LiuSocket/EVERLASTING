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
		/** 0��idle */
		EA_BONE_IDLE,
		/** 0��idle ת 1��idle */
		EA_BONE_IDLE_ADD_0,
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
		/** ����� */
		EA_MORPH_HALF,
		/** ���͵�idle���� */
		EA_MORPH_IDLE,
		/** ���͡����� */
		EA_MORPH_AA,
		/** ���͡�Ŷ�� */
		EA_MORPH_OO,
		/** ���ȵ������۾��������� */
		EA_MORPH_SURPRISE
	};
	/*************************************************************************
	 Structs
	*************************************************************************/
	/*!
	 *  @struct SGMAnimData
	 *  @brief �������ݽṹ��
	 */
	struct SGMAnimData
	{
		/** @brief ���� */
		SGMAnimData(EGMANIMATION_BONE eAnim)
			: eAnimation(eAnim)
		{}
		/**
		* @brief ��Ȩ�����õø��ӽ�Ŀ��
		* @param fDeltaTime ���ʱ�䣬��λ����
		* @param fSpeed �ӽ�Ŀ��Ȩ�ص��ٶȱ�������Ҫ�������������[1,5]
		* @return bool:	�����ǰȨ�ص���Ŀ��Ȩ�أ��򷵻�false�����򷵻�true
		*/
		bool SetWeightCloserToTarget(const float fDeltaTime, const float fSpeed)
		{
			if (fWeightNow == fWeightTarget) return false;

			// ��������ݽ�ɫ��������ٶ�ϵ��
			float fStep = osg::clampTo(fSpeed, 1.0f, 5.0f)*fDeltaTime*abs(fWeightTarget - fWeightSource);

			if (fWeightNow < fWeightTarget)
			{
				fWeightNow += fStep;
				fWeightNow = fmin(fWeightNow, fWeightTarget);
			}
			else
			{
				fWeightNow -= fStep;
				fWeightNow = fmax(fWeightNow, fWeightTarget);
			}
			return true;
		}
		/**
		* @brief ��Ȩ����ԴȨ�غ�Ŀ��Ȩ��֮������ֵ���
		* @param fMix ���ϵ�����ο�glsl��mix����
		* @return bool:	�����ǰȨ�ص���Ŀ��Ȩ�أ��򷵻�false�����򷵻�true
		*/
		inline bool SetWeightMix(const float fMix)
		{
			if (fWeightNow == fWeightTarget) return false;
			fWeightNow = fWeightSource * (1 - fMix) + fWeightTarget * fMix;
			return true;
		}

		float fWeightSource = 0;
		float fWeightNow = 0;
		float fWeightTarget = 0;
		EGMANIMATION_BONE eAnimation = EA_BONE_HEAD_L;
		bool bAnimOn = false;
	};

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
		/** @brief �����۾��ı�ýڵ��vector */
		void InitEyeTransform(std::vector<osg::ref_ptr<osg::Transform>>& v);

		/**
		* @brief ��ʾ/����Ŀ�꣬Ŀ�겻�ɼ��󣬽�ɫ�������Ǹ������ɻ�Ƭ��
		* @param bVisible ��ʾ/����
		*/
		inline void SetLookTargetVisible(bool bVisible)
		{
			m_bTargetVisible = bVisible;
			// ���Ŀ��ͻȻ��ʧ����ʼ��ѰĿ�꣬����ʱ
			if (!bVisible) m_fSeekTargetTime = 0;
		}
		/**
		* @brief ���Ŀ��ɼ���������ע��Ŀ��λ��
		* @param vTargetWorldPos Ŀ��������ռ����꣬��λ��cm
		*/
		inline void SetLookTargetPos(const osg::Vec3d& vTargetWorldPos)
		{
			if(m_bTargetVisible)
				m_vTargetWorldPos = vTargetWorldPos;
		}

	private:
		void _InnerUpdate(const double dDeltaTime);
		/** @brief ��ʱ����գ��״̬ */
		void _InnerUpdateBlink(const double dDeltaTime);
		/** @brief ��ʱ�����촽״̬ */
		void _InnerUpdateLip(const double dDeltaTime);
		/** @brief �ı�idle״̬ */
		void _ChangeIdle(const double dDeltaTime);
		/** @brief �ı�ע�ӷ��� */
		void _ChangeLookDir(const double dDeltaTime);
		/** @brief ע��Ŀ�� */
		void _ChangeLookAtTarget(const double dDeltaTime);
		/** @brief �Ĵ����� */
		void _ChangeLookAround(const double dDeltaTime);
		/** @brief �ı�Ŀ�궯�� */
		void _ChangeTargetAnimation(const float fTargetHeading, const float fTargetPitch);

		/** @brief ÿ֡����idle���� */
		void _UpdateIdle(const double dDeltaTime);
		/** @brief ÿ֡����תͷ�����Ĺ���״̬ */
		void _UpdateLookAnimation(const double dDeltaTime);
		/** @brief ÿ֡����תͷ�����Ĺ���״̬ */
		void _UpdateLookAt(const double dDeltaTime);
		/** @brief ÿ֡�����Ĵ����������Ĺ���״̬ */
		void _UpdateLookAround(const double dDeltaTime);
		/** @brief ÿ֡���������� */
		void _UpdateEye(const double dDeltaTime);

		/**
		* @brief ���������Ҷ�����Ȩ�����õø��ӽ�Ŀ��
		* @param fDeltaTime ���ʱ�䣬��λ����
		* @param fSpeed �ӽ�Ŀ��Ȩ�ص��ٶȱ�������Ҫ�������������[1,5]
		* @return bool:	����������Ҷ����ĵ�ǰȨ�ض�����Ŀ��Ȩ�أ��򷵻�false�����򷵻�true
		*/
		bool _SetWeightCloserToTarget(const float fDeltaTime, const float fSpeed);
		/**
		* @brief ���������Ҷ�����Ȩ����ԴȨ�غ�Ŀ��Ȩ��֮������ֵ���
		* @param fMix ���ϵ�����ο�glsl��mix����
		* @return bool:	����������Ҷ����ĵ�ǰȨ�ض�����Ŀ��Ȩ�أ��򷵻�false�����򷵻�true
		*/
		bool _SetWeightMix(const float fMix);

		void _UpdateAnimationWeight();
		void _StopAnimation();

		/**
		* @brief ��������������۾��ķ���
		* @param fHeading: ����ƫ���ǣ������Ҹ�����λ����
		* @param fPitch: �������ǣ������¸�����λ����
		*/
		inline void _SetEyeFinalDir(float fHeading, float fPitch)
		{
			m_fEyeBallFinalHeading = osg::DegreesToRadians(fHeading);
			m_fEyeBallFinalPitch = osg::DegreesToRadians(fPitch);
		}

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

		float m_fIdleDuration = 5.0f;							//!< idle�������ڣ���λ����
		float m_fIdleAddDuration = 1.0f;						//!< idle���Ӷ������ڣ���λ����
		float m_fIdleTime = 0.0f;								//!< idle���������˶೤ʱ�䣬��λ����
		float m_fIdleAddTime = 0.0f;							//!< idle���Ӷ��������˶೤ʱ�䣬��λ����

		float m_fSeekTargetTime = 0.0f;							//!< ����Ŀ������������˶೤ʱ�䣬��λ����
		float m_fLookDuration = 2.0f;							//!< ע�����ڣ�����ע��Ŀ�꣩����λ����
		float m_fTurnDuration = 1.0f;							//!< תͷ���ڣ���λ����
		float m_fFastTurnDuration = 0.5f;						//!< ����תͷ���ڣ���λ����
		float m_fMixTime = 0.0f;								//!< ��ǰ�����������ʱ�䣬��λ����

		float m_fTargetHeading = 0.0f;							//!< �۾�ƫ���ǣ������Ҹ�����λ����
		float m_fTargetPitch = 0.0f;							//!< �۾������ǣ������¸�����λ����

		SGMAnimData m_animL = SGMAnimData(EA_BONE_HEAD_L);		//!< left������Ȩ��
		SGMAnimData m_animR = SGMAnimData(EA_BONE_HEAD_R);		//!< right������Ȩ��
		SGMAnimData m_animU = SGMAnimData(EA_BONE_HEAD_U);		//!< up������Ȩ��
		SGMAnimData m_animD = SGMAnimData(EA_BONE_HEAD_D);		//!< down������Ȩ��

		EGMANIMATION_BONE m_eNextHeadingAnim = EA_BONE_HEAD_L;	//!< ��һ��ת�򶯻�
		EGMANIMATION_BONE m_eNextPitchAnim = EA_BONE_HEAD_U;	//!< ��һ����������

		std::vector<std::string> m_strBoneAnimNameVec;			//!< ������������vector
		std::vector<std::string> m_strMorphAnimNameVec;			//!< ���ζ�������vector

		std::vector<osg::ref_ptr<osg::Transform>> m_pEyeTransVector; //!< ����ı�ýڵ�
		std::vector<osg::Matrix> m_mEyeTransVector;				//!< ����ı�þ���
		float m_fEyeBallFinalHeading = 0.0f;					//!< ���������ƫ���ǣ������Ҹ�����λ������
		float m_fEyeBallFinalPitch = 0.0f;						//!< �����������ǣ������¸�����λ������
		float m_fEyeBallHeading = 0.0f;							//!< ����ǰƫ���ǣ������Ҹ�����λ������
		float m_fEyeBallPitch = 0.0f;							//!< ����ǰ�����ǣ������¸�����λ������

		//!< ���棬[0.0, 1.0]��0.0 == ��������Ŀ�ꣻ1.0 == Ũ����ȫ��Ŀ������
		float m_fInterest = 0.0f;
		//!< ��ŭ��[0.0, 1.0]��0.0 == ƽ����1.0 == �ǳ���ŭ
		float m_fAngry = 0.0f;
		//!< ���£�[0.0, 1.0]��0.0 == ƽ����1.0 == �е��ֲ�
		float m_fScared = 0.0f;

		bool m_bDisdain = false;								//!< �Ƿ��ڱ��ӣ��Ժ��Ǽ����Ը�
		bool m_bTargetVisible = false;							//!< ע��Ŀ���Ƿ�ɼ�

		osg::Vec3d m_vTargetWorldPos = osg::Vec3d(0,-30,0);		//!< Ŀ��������ռ����꣬��λ��cm
		osg::Vec3d m_vTargetLastWorldPos = osg::Vec3d(0, -30, 0);//!< Ŀ�����һ��ָ��������ռ����꣬��λ��cm
		osg::Vec3d m_vTargetLastVelocity = osg::Vec3d(0, 0, 0);	//!< Ŀ�����һ�ε��ٶȣ���λ��cm/s
	};
}	// GM
