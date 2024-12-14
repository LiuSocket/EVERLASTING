//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2040, LT
/// All rights reserved.
///
/// @file		GMMaterial.h
/// @brief		GMEngine - Material manager
/// @version	1.0
/// @author		LiuTao
/// @date		2024.02.04
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommon.h"
#include "GMCommonUniform.h"
#include <osg/Texture2D>
#include <osg/Texture2DArray>
#include <osg/BufferIndexBinding>
#include <osg/Vec2>
#include <random>

namespace GM
{
	/*************************************************************************
	 Structs
	*************************************************************************/

	/*************************************************************************
	Class
	*************************************************************************/
	class CGMDispatchCompute;

	/*!
	*  @Class CGMMaterial
	*  @brief DOF manager for GM
	*/
	class CGMMaterial
	{
	// ����
	public:
		/** @brief ���� */
		CGMMaterial();
		/** @brief ���� */
		~CGMMaterial();

		/** @brief ��ʼ�� */
		bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData, CGMCommonUniform* pCommonUniform);

		/** @brief ���� PBR����
		* @param pNode ��Ҫ�޸Ĳ��ʵĽڵ�ָ��
		*/
		void SetPBRMaterial(osg::Node* pNode);
		/**
		* @brief ���� ���ࣨ����Ƥ�����۾�������
		* @param pNode ��Ҫ�޸Ĳ��ʵĽڵ�ָ��
		*/
		void SetHumanMaterial(osg::Node* pNode);
		/**
		* @brief ���� �α���ɢ�����
		* @param pNode/pGeom ��Ҫ�޸Ĳ��ʵĽڵ�ָ��
		*/
		void SetSSSMaterial(osg::Node* pNode);
		/**
		* @brief ���� �۾�����
		* @param pNode/pGeom ��Ҫ�޸Ĳ��ʵĽڵ�ָ��
		*/
		void SetEyeMaterial(osg::Node* pNode);
		/**
		* @brief ���� ��������
		* @param pNode ��Ҫ�޸Ĳ��ʵĽڵ�ָ��
		*/
		void SetBackgroundMaterial(osg::Node* pNode);

		/**
		* @brief ����shader
		* @param pStateSet ��Ҫ���shader��״̬��
		* @param eMaterial ��������
		*/
		void SetShader(osg::StateSet* pStateSet, EGMMaterial eMaterial);

	private:
		/**
		* @brief �жϵ�ǰ����Ԫ�Ƿ��Ѿ���ռ�ã������ռ�ã���++��������ռ�õ�����Ԫ��
		* @param iUnit ����Ԫ
		* @return bool: �����ռ���򷵻�false�����򷵻�true
		*/
		bool _PlusUnitUsed(int& iUnit);

	// ����
	private:
		SGMKernelData* m_pKernelData = nullptr;					//!< �ں�����
		SGMConfigData* m_pConfigData = nullptr;					//!< ��������
		CGMCommonUniform* m_pCommonUniform = nullptr;			//!< ����Uniform

		std::string								m_strModelShaderPath;					//!< ģ��shader·��
		std::string								m_strDefTexPath;						//!< ģ�������ͼ��Ĭ��·��
		std::default_random_engine				m_iRandom;								//!< ���ֵ
		osg::ref_ptr<osgDB::Options>			m_pDDSOptions;							//!< dds���������
		// Ĭ�ϵĸ������ʵ���ͼ�����ڲ�������Ԫ
		std::vector<osg::ref_ptr<osg::Texture2D>> m_pPBRTexVector;						//!< PBRģ�͵�����ԪĬ����ͼ

		// ������ͼ
		osg::ref_ptr<osg::Texture2D>			m_pRainRippleTex;						//!< ˮ������
		osg::ref_ptr<osg::Texture2D>			m_pWetNormalTex;						//!< ��ʪ���淨����ͼ
		osg::ref_ptr<osg::Texture2D>			m_pNoiseTex;							//!< ������ͼ
		osg::ref_ptr<osg::Texture2D>			m_pSnowTex;								//!< ��ѩ��ͼ
		osg::ref_ptr<osg::Texture2D>			m_pSandTex;								//!< ɳ����ͼ
		osg::ref_ptr<osg::Texture2D>			m_pEnvProbeTex;							//!< ����̽����ͼ

		std::vector<osg::ref_ptr<osg::TextureCubeMap>> m_pCubeMapVector; //!< cubemap���飬6������6��level
		std::vector<osg::ref_ptr<CGMDispatchCompute>> m_pMipmapComputeVec; // �����Զ���mipmap�ļ�����ɫ���ڵ�
	};

}	// GM
