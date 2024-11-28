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
	// 函数
	public:
		/** @brief 构造 */
		CGMLight();
		/** @brief 析构 */
		~CGMLight();

		/** @brief 初始化 */
		bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData);
		/** @brief 加载 */
		bool Load();
		/** @brief 保存 */
		bool Save();
		/** @brief 重置 */
		bool Reset();
		/** @brief 更新 */
		bool Update(double dDeltaTime);
		/** @brief 更新(在主相机更新姿态之后) */
		bool UpdatePost(double dDeltaTime);

		/** @brief 增加需要阴影的节点 */
		void AddShadowNode(osg::Node* pNode);

	private:
		void _InnerUpdate(const double dDeltaTime);

	// 变量
	private:
		SGMKernelData* m_pKernelData;					//!< 内核数据
		SGMConfigData* m_pConfigData;					//!< 配置数据

		//!< osg的射灯
		osg::ref_ptr<osg::LightSource>			m_pLightSource;
		//!< 阴影相机
		osg::ref_ptr<osg::Camera>				m_pShadowCamera;
		//!< 阴影贴图
		osg::ref_ptr<osg::Texture2D>			m_pShadowTexture;
	};
}	// GM
