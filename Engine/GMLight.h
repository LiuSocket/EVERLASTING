//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMLight.h
/// @brief		GMEngine - Light Module
/// @version	1.0
/// @author		LiuTao
/// @date		2024.10.21
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommon.h"
#include "GMKernel.h"

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
	 *  @brief GM Light Module
	 */
	class CGMLight
	{
	// ����
	public:
		/** @brief ���� */
		CGMLight();
		/** @brief ���� */
		~CGMLight();

		/** @brief ��ʼ�� */
		bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData);
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

	private:
		void _InnerUpdate(const double dDeltaTime);

	// ����
	private:
		SGMKernelData* m_pKernelData;					//!< �ں�����
		SGMConfigData* m_pConfigData;					//!< ��������

		//!< osg�����
		osg::ref_ptr<osg::LightSource>		m_pLightSource;
	};
}	// GM