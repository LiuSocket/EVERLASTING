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
	*  @brief 各个模块常用的Uniform
	*/
	class CGMCommonUniform
	{
		// 函数
	public:
		/** @brief 构造 */
		CGMCommonUniform();
		/** @brief 析构 */
		~CGMCommonUniform();
		/** @brief 初始化 */
		void Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData);
		/** @brief 更新(在主相机更新姿态之前) */
		void Update(double dDeltaTime);
		/** @brief 更新(在主相机更新姿态之后，所有其他模块UpdateLater之前) */
		void UpdateLater(double dDeltaTime);
		/**
		* 修改屏幕尺寸时调用此函数
		* @param width: 屏幕宽度
		* @param height: 屏幕高度
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

		// 变量
	private:
		SGMKernelData* m_pKernelData;				//!< 内核数据

		osg::ref_ptr<osg::Uniform> m_vScreenSizeUniform;			//!< vec3(屏幕长，屏幕宽，RTT比例)
		osg::ref_ptr<osg::Uniform> m_fTimeUniform;					//!< 三维渲染的持续时间，不是程序运行时长，单位：秒
		osg::ref_ptr<osg::Uniform> m_mDeltaVPMatrixUniform;			//!< 相机插值VP矩阵
		osg::ref_ptr<osg::Uniform> m_mMainInvProjUniform;			//!< 主相机的ProjectionMatrix的逆矩阵
		osg::ref_ptr<osg::Uniform> m_vEyeFrontDirUniform;			//!< 主相机前方单位向量，在世界空间
		osg::ref_ptr<osg::Uniform> m_vEyeRightDirUniform;			//!< 主相机右方单位向量，在世界空间
		osg::ref_ptr<osg::Uniform> m_vEyeUpDirUniform;				//!< 主相机上方单位向量，在世界空间
		osg::ref_ptr<osg::Uniform> m_vViewUpUniform;				//!< 眼点view空间Up向量，指向天空

		double m_fRenderingTime;									//!< 三维渲染的持续时间，不是程序运行时长
	};

}	// GM
