//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMCommonUniform.h
/// @brief		Galaxy-Music Engine - Common Uniform
/// @version	1.0
/// @author		LiuTao
/// @date		2022.08.21
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommon.h"
#include "GMKernel.h"
#include <osg/Uniform>

namespace GM
{
	/*************************************************************************
	 Struct
	*************************************************************************/

	/*************************************************************************
	 Class
	*************************************************************************/
	/*!
	*  @class CGMCommonUniform
	*  @brief ����ģ�鳣�õ�Uniform
	*/
	class CGMCommonUniform
	{
		// ����
	public:
		/** @brief ���� */
		CGMCommonUniform();
		/** @brief ���� */
		~CGMCommonUniform();
		/** @brief ��ʼ�� */
		void Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData);
		/** @brief ����(�������������̬֮ǰ) */
		void Update(double dDeltaTime);
		/** @brief ����(�������������̬֮����������ģ��UpdatePost֮ǰ) */
		void UpdatePost(double dDeltaTime);
		/**
		* �޸���Ļ�ߴ�ʱ���ô˺���
		* @param width: ��Ļ���
		* @param height: ��Ļ�߶�
		*/
		void ResizeScreen(const int width, const int height);

		inline osg::Uniform*const GetScreenSize() const { return m_vScreenSizeUniform.get(); }
		inline osg::Uniform*const GetTime() const { return m_fTimeUniform.get(); }

		inline osg::Uniform* const GetDeltaVPMatrix() const { return m_mDeltaVPMatrixUniform.get(); }
		inline void SetDeltaVPMatrix(const osg::Matrixf m) { m_mDeltaVPMatrixUniform->set(m); }

		inline osg::Uniform* const GetMainInvProjMatrix() const { return m_mMainInvProjUniform.get(); }
		inline void SetMainInvProjMatrix(const osg::Matrixf m) { m_mMainInvProjUniform->set(m); }

		inline osg::Uniform* const GetEyeFrontDir() const { return m_vEyeFrontDirUniform.get(); }
		inline void SetEyeFrontDir(const osg::Vec3f v) { m_vEyeFrontDirUniform->set(v); }

		inline osg::Uniform* const GetEyeRightDir() const { return m_vEyeRightDirUniform.get(); }
		inline void SetEyeRightDir(const osg::Vec3f v) { m_vEyeRightDirUniform->set(v); }

		inline osg::Uniform* const GetEyeUpDir() const { return m_vEyeUpDirUniform.get(); }
		inline void SetEyeUpDir(const osg::Vec3f v) { m_vEyeUpDirUniform->set(v); }

		inline osg::Uniform* const GetViewUp() const { return m_vViewUpUniform.get(); }
		inline void SetViewUp(const osg::Vec3f v) { m_vViewUpUniform->set(v); }

		// ����
	private:
		SGMKernelData* m_pKernelData;				//!< �ں�����

		osg::ref_ptr<osg::Uniform> m_vScreenSizeUniform;			//!< vec3(��Ļ������Ļ��RTT����)
		osg::ref_ptr<osg::Uniform> m_fTimeUniform;					//!< ��ά��Ⱦ�ĳ���ʱ�䣬���ǳ�������ʱ������λ����
		osg::ref_ptr<osg::Uniform> m_mDeltaVPMatrixUniform;			//!< �����ֵVP����
		osg::ref_ptr<osg::Uniform> m_mMainInvProjUniform;			//!< �������ProjectionMatrix�������
		osg::ref_ptr<osg::Uniform> m_vEyeFrontDirUniform;			//!< �����ǰ����λ������������ռ�
		osg::ref_ptr<osg::Uniform> m_vEyeRightDirUniform;			//!< ������ҷ���λ������������ռ�
		osg::ref_ptr<osg::Uniform> m_vEyeUpDirUniform;				//!< ������Ϸ���λ������������ռ�
		osg::ref_ptr<osg::Uniform> m_vViewUpUniform;				//!< �۵�view�ռ�Up������ָ�����

		double m_fRenderingTime;									//!< ��ά��Ⱦ�ĳ���ʱ�䣬���ǳ�������ʱ��
	};

}	// GM
