//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2040, LT
/// All rights reserved.
///
/// @file		GMAnimation.h
/// @brief		GMEngine - animation manager
/// @version	1.0
/// @author		LiuTao
/// @date		2024.1.11
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "../GMStructs.h"

#include <osg/Node>
#include <osgAnimation/BasicAnimationManager>

namespace GM
{
	/*************************************************************************
	 Macro Defines
	*************************************************************************/
	#define GM_ANIMATION                    CGMAnimation::getSingleton()

	/*************************************************************************
	Class
	*************************************************************************/
	class CAnimationPlayer;

	/*!
	*  @Class CGMAnimation
	*  @brief Animation manager for GM
	*/
	class CGMAnimation : public CGMSingleton<CGMAnimation>
	{
	// ����
	protected:
		/** @brief ���� */
		CGMAnimation();
		/** @brief ���� */
		virtual ~CGMAnimation();

	// ����
	public:
		/** @brief ��ȡ���� */
		static CGMAnimation& getSingleton(void);

		/** @brief �ͷ� */
		void Release();
		/** @brief ���� */
		bool Reset();

		/**
		* @brief ��ȡ���ص�ģ���еĶ������������������ζ�����
		* @param strName: ģ���ڳ����е�����
		* @param pNode: ģ�ͽڵ�ָ��
		* @return bool �ɹ�OK��ʧ��Fail
		*/
		bool AddAnimation(const std::string& strName, osg::Node* pNode);

		/**
		* @brief �Ƴ����ص�ģ���еĶ���
		* @param strName: ģ���ڳ����е�����
		* @return bool �ɹ�OK��ʧ��Fail
		*/
		bool RemoveAnimation(const std::string& strName);

		/**
		* @brief ��ȡָ��ģ�͵Ķ������ܣ��������������ζ������Ƿ���
		* @param strName: ģ���ڳ����е�����
		* @return bool: �Ƿ����ö���
		*/
		bool GetAnimationEnable(const std::string& strName);

		/**
		* @brief ���ö�������ʱ��
		* @param strModelName ģ������
		* @param fDuration ����ʱ������λ����
		* @param strAnimationName �������ƣ������붯�����ƾ�ֻ�޸ĵ�һ��������һ�����ƵĶ���
		* @return bool �ɹ����� true��ʧ�ܷ��� false
		*/
		bool SetAnimationDuration(const std::string& strModelName, const float fDuration, const std::string& strAnimationName = "");
		/**
		* @brief ��ȡ��������ʱ��
		* @param strModelName ģ������
		* @param strAnimationName ��������
		* @return float ����ʱ������λ����
		*/
		float GetAnimationDuration(const std::string& strModelName, const std::string& strAnimationName);
		/**
		* @brief ���ö�������ģʽ
		* @param strModelName ģ������
		* @param ePlayMode ����ģʽ
		* @param strAnimationName �������ƣ������붯�����ƾ�ֻ�޸ĵ�һ��������һ�����ƵĶ���
		* @return bool �ɹ����� true��ʧ�ܷ��� false
		*/
		bool SetAnimationMode(const std::string& strModelName, EGMPlayMode ePlayMode, const std::string& strAnimationName = "");
		/**
		* @brief ��ȡ��������ģʽ
		* @param strModelName ģ������
		* @param strAnimationName ��������
		* @return EGMPlayMode ����ģʽ
		*/
		EGMPlayMode GetAnimationMode(const std::string& strModelName, const std::string& strAnimationName);
		/**
		* @brief ���ö������ȼ�
		* @param strModelName ģ������
		* @param iPriority �������ȼ���[0,100]����ֵԽ�����ȼ�Խ��
		* @param strAnimationName �������ƣ������붯�����ƾ�ֻ�޸ĵ�һ��������һ�����ƵĶ���
		* @return bool �ɹ����� true��ʧ�ܷ��� false
		*/
		bool SetAnimationPriority(const std::string& strModelName, int iPriority, const std::string& strAnimationName = "");
		/**
		* @brief ��ȡ�������ȼ�
		* @param strModelName ģ������
		* @param strAnimationName ��������
		* @return int �������ȼ�
		*/
		int GetAnimationPriority(const std::string& strModelName, const std::string& strAnimationName);
		/**
		* @brief ���ö���Ȩ��
		* @param strModelName ģ������
		* @param fWeight �������Ȩ�أ�[0.0,1.0]����������ɸ���Ȩ�ػ��
		* @param strAnimationName �������ƣ������붯�����ƾ����õ�һ��������һ�����ƵĶ���
		* @return bool �ɹ����� true��ʧ�ܷ��� false
		*/
		bool SetAnimationWeight(const std::string& strModelName, float fWeight, const std::string& strAnimationName = "");
		/**
		* @brief ��ȡ����Ȩ��
		* @param strModelName ģ������
		* @param strAnimationName ��������
		* @return float �������Ȩ��
		*/
		float GetAnimationWeight(const std::string& strModelName, const std::string& strAnimationName);

		/**
		* @brief ���Ŷ���
		* @param strModelName ģ������
		* @param strAnimationName �������ƣ������붯�����ƾͲ��ŵ�һ��������һ�����ƵĶ���
		* @return bool �ɹ����� true��ʧ�ܷ��� false
		*/
		bool SetAnimationPlay(const std::string& strModelName, const std::string& strAnimationName = "");
		/**
		* @briefֹͣ��������������붯�����ƣ���ֹͣ���ж���
		* @param strModelName ģ������
		* @param strAnimationName �������ƣ������붯�����ƾ�ֹͣ���ж���
		* @return bool �ɹ����� true��ʧ�ܷ��� false
		*/
		bool SetAnimationStop(const std::string& strModelName, const std::string& strAnimationName = "");
		/**
		* @brief ��ͣ����
		* @param strModelName ģ������
		* @param strAnimationName �������ƣ���������Ϊ��ʱ���������ж���
		* @return bool �ɹ����� true��ʧ�ܷ��� false
		*/
		bool SetAnimationPause(const std::string& strModelName, const std::string& strAnimationName);
		/**
		* @brief �������Ŷ���
		* @param strModelName ģ������
		* @param strAnimationName ��������
		* @param fWeight �������Ȩ�أ�[0.0,1.0]��0.0��ʾֹͣ��1.0��ʾ���ţ������ɸ���Ȩ�ػ��
		* @return bool �ɹ����� true��ʧ�ܷ��� false
		*/
		bool SetAnimationResume(const std::string& strModelName, const std::string& strAnimationName, float fWeight = 1.0f);

		/**
		* @brief: ��ȡĳ��ģ�͵Ķ����б�
		* @param strModelName ģ������
		* @param vAnimationList �����б�
		* @return bool �ɹ�OK��ʧ��Fail
		*/
		bool GetAnimationList(const std::string& strModelName, std::vector<std::string>& vAnimationList);

		/**
		* @brief: ��ȡĳ��ģ�͵�ĳ�������Ƿ��ڲ���
		* @param strModelName ģ������
		* @param vAnimationList �����б�
		* @return bool ��OK������Fail
		*/
		bool IsAnimationPlaying(const std::string& strModelName, const std::string& strAnimationName);

	private:
		/**
		* @brief ��ȡ�����������������ģ���еĻ���ÿ��ģ�ͻ��Ӧһ������������
		* @param strName ģ���ڳ����е�����
		* @return CAnimationPlayer* ����������ָ��
		*/
		CAnimationPlayer* _GetPlayerByModelName(const std::string& strModelName);
		/**
		* @brief ��Ӷ���������
		* @param strPlayerName ����������
		* @param pPlayer ����������ָ��
		* @return bool �ɹ�����true��ʧ�ܷ���false
		*/
		bool _AddPlayer(const std::string& strPlayerName, CAnimationPlayer* pPlayer);
		/**
		* @brief �Ƴ�����������
		* @param strPlayerName ����������
		* @return bool �ɹ�����true��ʧ�ܷ���false
		*/
		bool _RemovePlayer(const std::string& strPlayerName);
		/**
		* @brief ������ж���������
		* @return bool �ɹ�����true��ʧ�ܷ���false
		*/
		bool _ClearPlayer();
		/**
		* @brief ��ȡ����������
		* @param strPlayerName ����������
		* @return CAnimationPlayer* ����������ָ��
		*/
		CAnimationPlayer* _GetPlayer(const std::string& strPlayerName);
		/**
		* @brief ��ȡ����������
		* @param strModelName ģ���ڳ����е�����
		* @return std::string ����������
		*/
		std::string _GetPlayerName(const std::string& strModelName);

	private:
		//!< ����������
		std::map<std::string, osg::ref_ptr<CAnimationPlayer>>	m_playerMap;
	};

}	// GM
