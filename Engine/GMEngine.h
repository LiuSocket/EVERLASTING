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

	/*************************************************************************
	Enums
	*************************************************************************/

	/*************************************************************************
	Structs
	*************************************************************************/

	/*************************************************************************
	Class
	*************************************************************************/

	class CGMPost;
	class CGMModel;
	class CGMCharacter;
	class CGMBaseManipulator;
	class CGMAudio;

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
		* @brief 修改屏幕尺寸时调用此函数
		* @param iW: 屏幕宽度
		* @param iH: 屏幕高度
		*/
		void ResizeScreen(const int iW, const int iH);
		/**
		* @brief 显示/隐藏目标
		* @param bVisible 显示/隐藏
		*/
		void SetLookTargetVisible(bool bVisible);
		/**
		* @brief 设置注视目标位置
		* @param vTargetScreenPos 目标点的屏幕XY坐标
		*/
		void SetLookTargetPos(const osg::Vec2f& vTargetScreenPos);
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
		
		/**
		* @brief 开启“欢迎效果”
		* 每次开启软件，调用此函数以实现“欢迎功能”
		*/
		void Welcome();
		/**
		* @brief “欢迎效果”是否结束
		* @return bool 结束则返回true，否则false
		*/
		bool IsWelcomeFinished() const;

		bool Play();

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

		SGMKernelData*						m_pKernelData = nullptr;		//!< 内核数据
		SGMConfigData*						m_pConfigData = nullptr;		//!< 配置数据
		CGMBaseManipulator*					m_pManipulator = nullptr;		//!< 基础操作器

		bool								m_bInit = false;                //!< 初始化标志
		bool								m_bRendering = true;            //!< 是否渲染
		double								m_dTimeLastFrame = 0.0;         //!< 上一帧时间
		float								m_fDeltaStep = 0.0f;            //!< 单位s
		float								m_fConstantStep = 0.1f;         //!< 等间隔更新的时间,单位s

		CGMModel*							m_pModel = nullptr;				//!< 模型模块
		CGMCharacter*						m_pCharacter = nullptr;			//!< 角色模块
		CGMAudio*							m_pAudio = nullptr;				//!< 音频模块
		CGMPost*							m_pPost = nullptr;				//!< 后期模块

		osg::ref_ptr<osg::Texture2D>		m_pSceneTex = nullptr;			//!< 主场景颜色图
		osg::ref_ptr<osg::Texture2D>		m_pBackgroundTex = nullptr;		//!< 背景颜色图
		osg::ref_ptr<osg::Texture2D>		m_pForegroundTex = nullptr;		//!< 前景颜色图
	};
}	// GM