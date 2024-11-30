//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMModel.h
/// @brief		GMEngine - Model Module
/// @version	1.0
/// @author		LiuTao
/// @date		2024.10.12
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommon.h"
#include "GMCommonUniform.h"
#include "GMKernel.h"

#include <osg/Texture2D>

namespace GM
{
	/*************************************************************************
	 Enums
	*************************************************************************/

	/*************************************************************************
	 Structs
	*************************************************************************/

	/*************************************************************************
	 Class
	*************************************************************************/

	class CGMAnimation;
	class CGMMaterial;

	/*!
	 *  @class CGMModel��ϵͳ��λ������
	 *  @brief GM Model Module
	 */
	class CGMModel
	{
	// ����
	public:
		/** @brief ���� */
		CGMModel();
		/** @brief ���� */
		~CGMModel();

		/** @brief ��ʼ�� */
		bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData, CGMCommonUniform* pCommonUniform);
		/** @brief ���� */
		bool Load();
		/** @brief ���� */
		bool Save();
		/** @brief ���� */
		bool Reset();
		/** @brief ���� */
		bool Update(double dDeltaTime);
		/** @brief ����(�������������̬֮��) */
		bool UpdatePost(double dDeltaTime);
		/** @brief ���ģ�� */
		bool Add(const SGMModelData& sData);

		/* @brief ����Uniform */
		void SetUniform(
			osg::Uniform* pView2Shadow
		);

		/**
		* @brief ������߽���ģ�͵Ķ������ܣ��������������ζ�����
		* @param strName: ģ���ڳ����е�����
		* @param bEnable: �Ƿ����ö�������
		* @return bool �ɹ�OK��ʧ��Fail��ģ�Ͳ����ڷ���NotExist
		*/
		bool SetAnimationEnable(const std::string& strName, const bool bEnable);
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
		bool SetAnimationPriority(const std::string& strModelName, const int iPriority, const std::string& strAnimationName = "");
		/**
		* @brief ��ȡ�������ȼ�
		* @param strModelName ģ������
		* @param strAnimationName ��������
		* @return int �������ȼ�
		*/
		int GetAnimationPriority(const std::string& strModelName, const std::string& strAnimationName);
		/**
		* @brief ���Ŷ�������������붯������������Ȩ��Ϊ0.0����ֹͣ���ж���
		* @param strModelName ģ������
		* @param fWeight �������Ȩ�أ�[0.0,1.0]��0.0��ʾֹͣ��1.0��ʾ���ţ������ɸ���Ȩ�ػ��
		* @param strAnimationName �������ƣ������붯�����ƾͲ��ŵ�һ��������һ�����ƵĶ���
		* @return bool �ɹ����� true��ʧ�ܷ��� false
		*/
		bool SetAnimationPlay(const std::string& strModelName, const float fWeight, const std::string& strAnimationName = "");
		/**
		* @brief ��ͣ����
		* @param strModelName ģ������
		* @param strAnimationName �������ƣ������붯�����ƾ���ͣ���ж���
		* @return bool �ɹ����� true��ʧ�ܷ��� false
		*/
		bool SetAnimationPause(const std::string& strModelName, const std::string& strAnimationName = "");
		/**
		* @brief �������Ŷ���
		* @param strModelName ģ������
		* @param strAnimationName �������ƣ������붯�����ƾͲ��ŵ�һ��������һ�����ƵĶ���
		* @return bool �ɹ����� true��ʧ�ܷ��� false
		*/
		bool SetAnimationResume(const std::string& strModelName, const std::string& strAnimationName = "");

		/** @brief ��ȡ��Ӱģ�͸��ڵ� */
		inline osg::Node* GetRootNode() const
		{
			return m_pRootNode.get();
		}
		/* @brief ������Ӱͼ */
		void SetShadowMap(osg::Texture2D* pShadowMap);

	private:
		/**
		* @brief ���ز���
		* @param pNode ��Ҫ�޸Ĳ��ʵĽڵ�ָ��
		* @param sData ��Ҫ�޸ĵ�ģ����Ϣ
		* @return bool �ɹ����� true��ʧ�ܷ��� true
		*/
		bool _SetMaterial(osg::Node* pNode, const SGMModelData& sData);
	
		/**
		* @brief �������ƻ�ȡģ��
		* @param strModelName ģ������
		* @return osg::Node* ģ�ͽڵ�ָ��
		*/
		osg::Node* _GetNode(const std::string& strModelName);

		void _InnerUpdate(const double dDeltaTime);

	// ����
	private:
		SGMKernelData* m_pKernelData = nullptr;					//!< �ں�����
		SGMConfigData* m_pConfigData = nullptr;					//!< ��������
		CGMCommonUniform* m_pCommonUniform = nullptr;			//!< ����Uniform

		osg::ref_ptr<osg::Group>			m_pRootNode;
		std::map<std::string, SGMModelData>	m_pModelDataMap;	//!< ģ������map
		std::map<std::string, osg::ref_ptr<osg::Node>> m_pNodeMap;	//!< ģ�ͽڵ�map

		// �����ͼ��Ĭ��·��
		std::string							m_strDefTexPath = "Textures/";
		// ģ���ļ�Ĭ��·��
		std::string							m_strDefModelPath = "Models/";
		//!< dds���������
		osg::ref_ptr<osgDB::Options>		m_pDDSOptions;
		//!< ����������
		CGMAnimation*						m_pAnimationManager;
		//!< ���ʹ�����
		CGMMaterial*						m_pMaterial;

		osg::ref_ptr<osg::Texture2D>		m_pShadowTexture;		//!< ��Ӱ��ͼ
	};
}	// GM
