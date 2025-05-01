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

	// 当前音频播放模式
	enum EGMA_MODE
	{
		EGMA_MOD_SINGLE,			// 单曲循环
		EGMA_MOD_CIRCLE,			// 列表循环
		EGMA_MOD_RANDOM,			// 随机播放
		EGMA_MOD_ORDER				// 顺序播放
	};

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
		
		/** @brief 播放 */
		bool Play();
		/** @brief 暂停 */
		bool Pause();
		/** @brief 停止 */
		bool Stop();
		/** @brief 下一首 */
		bool Next();
		/** @brief 设置音量，0.0-1.0 */
		bool SetVolume(const float fVolume);
		/** @brief 获取音量，0.0-1.0 */
		float GetVolume() const;
		/**
		* @brief 设置播放模式
		* @param eMode:			播放模式（单曲循环、随机播放、列表循环等）
		* @return bool：		成功true， 失败false
		*/
		bool SetPlayMode(EGMA_MODE eMode);
		/**
		* @brief 获取播放模式	
		* @return EGMA_MODE：	播放模式（单曲循环、随机播放、列表循环等）
		*/
		inline EGMA_MODE GetPlayMode() const
		{
			return m_ePlayMode;
		}

		/**
		* @brief 获取当前音频文件名称
		* @return std::wstring 当前播放的音频文件名称，含后缀名，未播放则返回 L""
		*/
		std::wstring GetAudioName() const;

		/**
		* @brief 设置音频的播放位置，单位：ms
		* @param iTime: 音频的播放位置
		* @return bool 成功true， 失败false
		*/
		bool SetAudioCurrentTime(const int iTime);
		/**
		* @brief 获取音频的播放位置，单位：ms
		* @return int: 音频的播放位置
		*/
		int GetAudioCurrentTime() const;

		/**
		* @brief 获取音频的总时长，单位：ms
		* @return int: 音频的总时长
		*/
		int GetAudioDuration() const;
		/**
		* @brief 判断音频是否播放完毕
		* @return bool 完毕返回true，未完毕返回false
		*/
		bool IsAudioOver() const;

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

		/**
		* @brief 获取音频播放的顺序列表，back位置为最新的音频
		* @return std::vector<std::wstring>：	音频播放的顺序列表
		*/
		const std::vector<std::wstring> GetPlayingOrder() const;

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
		* @brief 播放下一首
		*/
		void _Next(const EGMA_MODE eMode);
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

		CGMModel*							m_pModel = nullptr;				//!< 模型模块
		CGMCharacter*						m_pCharacter = nullptr;			//!< 角色模块
		CGMAudio*							m_pAudio = nullptr;				//!< 音频模块
		CGMPost*							m_pPost = nullptr;				//!< 后期模块

		EGMA_MODE							m_ePlayMode = EGMA_MOD_SINGLE;	//!< 当前播放模式
		osg::ref_ptr<osg::Texture2D>		m_pSceneTex = nullptr;			//!< 主场景颜色图
		osg::ref_ptr<osg::Texture2D>		m_pBackgroundTex = nullptr;		//!< 背景颜色图
		osg::ref_ptr<osg::Texture2D>		m_pForegroundTex = nullptr;		//!< 前景颜色图
	};
}	// GM