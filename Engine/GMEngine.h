//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMVolumeBasic.h
/// @brief		Galaxy-Music Engine - GMEngine.h
/// @version	1.0
/// @author		LiuTao
/// @date		2020.12.10
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "GMCommon.h"
#include "GMKernel.h"
#include <random>

/*************************************************************************
Class
*************************************************************************/
class QWidget;

namespace GM
{
	/*************************************************************************
	Macro Defines
	*************************************************************************/
	#define GM_ENGINE					CGMEngine::getSingleton()
	#define GM_ENGINE_PTR				CGMEngine::getSingletonPtr()

	/*************************************************************************
	Enums
	*************************************************************************/

	/*************************************************************************
	Structs
	*************************************************************************/

	/*************************************************************************
	Class
	*************************************************************************/
	class CGMCommonUniform;
	class CGMPost;

	/*!
	*  @class CGMEngine
	*  @brief Galaxy-Music GMEngine
	*/
	class CGMEngine : public CGMSingleton<CGMEngine>
	{
		// 函数
	protected:
		/** @brief 构造 */
		CGMEngine();
		/** @brief 析构 */
		~CGMEngine();

	public:
		/** @brief 获取单例 */
		static CGMEngine& getSingleton(void);
		static CGMEngine* getSingletonPtr(void);

		/** @brief 初始化 */
		bool Init();
		/** @brief 释放 */
		void Release();
		/** @brief 更新 */
		bool Update();
		/** @brief 加载 */
		bool Load();
		/** @brief 保存 */
		bool Save();
		/**
		* 修改屏幕尺寸时调用此函数
		* @param iW: 屏幕宽度
		* @param iH: 屏幕高度
		*/
		void ResizeScreen(const int iW, const int iH);
		/**
		* @brief 开启/暂停渲染，最小化或者其他软件置顶时可以关闭渲染
		*  关闭是为了最小化时不浪费显卡资源
		* @param bEnable: 是否开启渲染
		*/
		inline void SetRendering(const bool bEnable)
		{
			m_bRendering = bEnable;
		}
		/**
		* @brief 是否开启渲染
		* @return bool: 是否开启渲染
		*/
		inline bool GetRendering() const
		{
			return m_bRendering;
		}
		
		/** @brief 创建视口(QT:QWidget) */
		CGMViewWidget* CreateViewWidget(QWidget* parent);

	private:
		/**
		* @brief 加载配置
		*/
		bool _LoadConfig();
		/**
		* @brief 初始化背景相关节点
		*/
		void _InitBackground();
		/**
		* @brief 初始化前景相关节点
		*/
		void _InitForeground();
		
		/**
		* @brief 间隔更新，一秒钟更新10次
		* @param updateStep 两次间隔更新的时间差，单位s
		*/
		void _InnerUpdate(const float updateStep);
		/** @brief 更新(在主相机更新姿态之后) */
		bool _UpdateLater(const double dDeltaTime);
		
		/**
		* @brief SGMVector3 转 osg::Vec3d
		* @param vGM:				输入的GM向量
		* @return osg::Vec3d:		输出的osg向量 */
		inline SGMVector3 _OSG2GM(const osg::Vec3d& vOSG) const
		{
			return SGMVector3(vOSG.x(), vOSG.y(), vOSG.z());
		}

		// 变量
	private:

		SGMKernelData*						m_pKernelData;				//!< 内核数据
		SGMConfigData*						m_pConfigData;				//!< 配置数据
		CGMCommonUniform*					m_pCommonUniform;			//!< 公用Uniform

		bool								m_bInit;					//!< 初始化标志
		bool								m_bRendering;				//!< 是否渲染
		double								m_dTimeLastFrame;			//!< 上一帧时间
		float								m_fDeltaStep;				//!< 单位s
		float								m_fConstantStep;			//!< 等间隔更新的时间,单位s

		CGMPost*							m_pPost;					//!< 后期模块

		std::default_random_engine			m_iRandom;

		osg::ref_ptr<osg::Texture2D>		m_pSceneTex;				//!< 主场景颜色图
		osg::ref_ptr<osg::Texture2D>		m_pBackgroundTex;			//!< 背景颜色图
		osg::ref_ptr<osg::Texture2D>		m_pForegroundTex;			//!< 前景颜色图
	};
}	// GM