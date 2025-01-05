//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMPost.h
/// @brief		Galaxy-Music Engine - GMPost
/// @version	1.0
/// @author		LiuTao
/// @date		2022.07.20
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

	/*!
	*  @class CGMPost
	*  @brief Galaxy-Music GMPost
	*/
	class CGMPost
	{
		// ����
	public:
		/** @brief ���� */
		CGMPost();

		/** @brief ���� */
		~CGMPost();

		/** @brief ��ʼ�� */
		bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData, CGMCommonUniform* pCommonUniform);
		/** @brief ���� */
		bool Update(double dDeltaTime);
		/** @brief ����(�������������̬֮��) */
		bool UpdateLater(double dDeltaTime);
		/** @brief ���� */
		bool Load();
		/**
		* @brief �޸���Ļ�ߴ�ʱ���ô˺���
		* @param width: ��Ļ���
		* @param height: ��Ļ�߶�
		* @return void
		*/
		void ResizeScreen(const int width, const int height);

		/**
		* @brief ��������
		* @param pSceneTex:			������ɫͼ
		* @param pBackgroundTex:	������ɫͼ
		* @param pForegroundTex:	ǰ����ɫͼ
		* @return bool �ɹ�true�� ʧ��false
		*/
		bool CreatePost(osg::Texture* pSceneTex,
			osg::Texture* pBackgroundTex,
			osg::Texture* pForegroundTex);

		/**
		* @brief ��������Ⱦ
		* @param bEnabled: ������ر�����Ⱦ
		* @param pVolumeTex: ����Ⱦͼ
		* @return bool �ɹ�true�� ʧ��false
		*/
		bool SetVolumeEnable(bool bEnabled, osg::Texture* pVolumeTex = nullptr);

	private:
		/**
		* @brief ������Ⱦ��
		* ��Ļ������С�������棬�Ⱦ���Ч��Ҫ��һЩ
		* @param width: ��Ч���εĿ��
		* @param height: ��Ч���εĸ߶�
		* @return osg::Geometry* ���ؼ��νڵ�ָ��
		*/
		osg::Geometry* _CreateScreenTriangle(const int width, const int height);
		/**
		* @brief ����Ⱦ������ߴ�
		* ��Ļ������С�������棬�Ⱦ���Ч��Ҫ��һЩ
		* @param width: ��Ч���εĿ��
		* @param height: ��Ч���εĸ߶�
		* @return void
		*/
		void _ResizeScreenTriangle(const int width, const int height);

		// ����
	private:
		SGMKernelData*										m_pKernelData;					//!< �ں�����
		SGMConfigData*										m_pConfigData;					//!< ��������
		CGMCommonUniform*									m_pCommonUniform;				//!< ����Uniform

		std::string											m_strShaderPath;				//!< post shader ��ɫ��·��

		osg::ref_ptr<osg::Camera>							m_pPostCam;						//!< ���������
		osg::ref_ptr<osg::Geode>							m_pPostGeode;					//!< ��Ⱦ�ڵ�

		osg::ref_ptr<osg::Texture>							m_pVolumeTex;					//!< ����Ⱦ��ɫͼ

		int													m_iPostUnit;					//!< ������嵱ǰ���õ�����Ԫ
		bool												m_bVolume;						//!< ����Ⱦ����
	};
}	// GM