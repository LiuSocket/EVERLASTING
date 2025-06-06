//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2040, LT
/// All rights reserved.
///
/// @file		GMAnimation.h
/// @brief		GMEngine - animation manager
/// @version	1.0
/// @author		LiuTao
/// @date		2024.1.11
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "../GMStructs.h"

#include <osg/Node>
#include <osgAnimation/BasicAnimationManager>

namespace GM
{
	/*************************************************************************
	 Macro Defines
	*************************************************************************/
	#define GM_ANIMATION                    CGMAnimation::getSingleton()

	/*************************************************************************
	Class
	*************************************************************************/
	class CAnimationPlayer;

	/*!
	*  @Class CGMAnimation
	*  @brief Animation manager for GM
	*/
	class CGMAnimation : public CGMSingleton<CGMAnimation>
	{
	// 函数
	protected:
		/** @brief 构造 */
		CGMAnimation();
		/** @brief 析构 */
		virtual ~CGMAnimation();

	// 函数
	public:
		/** @brief 获取单例 */
		static CGMAnimation& getSingleton(void);

		/** @brief 释放 */
		void Release();
		/** @brief 重置 */
		bool Reset();

		/**
		* @brief 获取加载的模型中的动画（骨骼动画、变形动画）
		* @param strName: 模型在场景中的名称
		* @param pNode: 模型节点指针
		* @return bool 成功OK，失败Fail
		*/
		bool AddAnimation(const std::string& strName, osg::Node* pNode);

		/**
		* @brief 移除加载的模型中的动画
		* @param strName: 模型在场景中的名称
		* @return bool 成功OK，失败Fail
		*/
		bool RemoveAnimation(const std::string& strName);

		/**
		* @brief 获取指定模型的动画功能（骨骼动画、变形动画）是否开启
		* @param strName: 模型在场景中的名称
		* @return bool: 是否启用动画
		*/
		bool GetAnimationEnable(const std::string& strName);

		/**
		* @brief 设置动画播放时长
		* @param strModelName 模型名称
		* @param fDuration 动画时长，单位：秒
		* @param strAnimationName 动画名称，不输入动画名称就只修改第一个或者上一个控制的动画
		* @return bool 成功返回 true，失败返回 false
		*/
		bool SetAnimationDuration(const std::string& strModelName, const float fDuration, const std::string& strAnimationName = "");
		/**
		* @brief 获取动画播放时长
		* @param strModelName 模型名称
		* @param strAnimationName 动画名称
		* @return float 动画时长，单位：秒
		*/
		float GetAnimationDuration(const std::string& strModelName, const std::string& strAnimationName);
		/**
		* @brief 设置动画播放模式
		* @param strModelName 模型名称
		* @param ePlayMode 播放模式
		* @param strAnimationName 动画名称，不输入动画名称就只修改第一个或者上一个控制的动画
		* @return bool 成功返回 true，失败返回 false
		*/
		bool SetAnimationMode(const std::string& strModelName, EGMPlayMode ePlayMode, const std::string& strAnimationName = "");
		/**
		* @brief 获取动画播放模式
		* @param strModelName 模型名称
		* @param strAnimationName 动画名称
		* @return EGMPlayMode 播放模式
		*/
		EGMPlayMode GetAnimationMode(const std::string& strModelName, const std::string& strAnimationName);
		/**
		* @brief 设置动画优先级
		* @param strModelName 模型名称
		* @param iPriority 动画优先级，[0,100]，数值越大优先级越高
		* @param strAnimationName 动画名称，不输入动画名称就只修改第一个或者上一个控制的动画
		* @return bool 成功返回 true，失败返回 false
		*/
		bool SetAnimationPriority(const std::string& strModelName, int iPriority, const std::string& strAnimationName = "");
		/**
		* @brief 获取动画优先级
		* @param strModelName 模型名称
		* @param strAnimationName 动画名称
		* @return int 动画优先级
		*/
		int GetAnimationPriority(const std::string& strModelName, const std::string& strAnimationName);
		/**
		* @brief 设置动画权重
		* @param strModelName 模型名称
		* @param fWeight 动画混合权重，[0.0,1.0]，多个动画可根据权重混合
		* @param strAnimationName 动画名称，不输入动画名称就设置第一个或者上一个控制的动画
		* @return bool 成功返回 true，失败返回 false
		*/
		bool SetAnimationWeight(const std::string& strModelName, float fWeight, const std::string& strAnimationName = "");
		/**
		* @brief 获取动画权重
		* @param strModelName 模型名称
		* @param strAnimationName 动画名称
		* @return float 动画混合权重
		*/
		float GetAnimationWeight(const std::string& strModelName, const std::string& strAnimationName);

		/**
		* @brief 播放动画
		* @param strModelName 模型名称
		* @param strAnimationName 动画名称，不输入动画名称就播放第一个或者上一个控制的动画
		* @return bool 成功返回 true，失败返回 false
		*/
		bool SetAnimationPlay(const std::string& strModelName, const std::string& strAnimationName = "");
		/**
		* @brief停止动画，如果不输入动画名称，就停止所有动画
		* @param strModelName 模型名称
		* @param strAnimationName 动画名称，不输入动画名称就停止所有动画
		* @return bool 成功返回 true，失败返回 false
		*/
		bool SetAnimationStop(const std::string& strModelName, const std::string& strAnimationName = "");
		/**
		* @brief 暂停动画
		* @param strModelName 模型名称
		* @param strAnimationName 动画名称，动画名称为空时，播放所有动画
		* @return bool 成功返回 true，失败返回 false
		*/
		bool SetAnimationPause(const std::string& strModelName, const std::string& strAnimationName);
		/**
		* @brief 继续播放动画
		* @param strModelName 模型名称
		* @param strAnimationName 动画名称
		* @param fWeight 动画混合权重，[0.0,1.0]，0.0表示停止，1.0表示播放，动画可根据权重混合
		* @return bool 成功返回 true，失败返回 false
		*/
		bool SetAnimationResume(const std::string& strModelName, const std::string& strAnimationName, float fWeight = 1.0f);

		/**
		* @brief: 获取某个模型的动画列表
		* @param strModelName 模型名称
		* @param vAnimationList 动画列表
		* @return bool 成功OK，失败Fail
		*/
		bool GetAnimationList(const std::string& strModelName, std::vector<std::string>& vAnimationList);

		/**
		* @brief: 获取某个模型的某个动画是否在播放
		* @param strModelName 模型名称
		* @param vAnimationList 动画列表
		* @return bool 是OK，不是Fail
		*/
		bool IsAnimationPlaying(const std::string& strModelName, const std::string& strAnimationName);

	private:
		/**
		* @brief 获取动画播放器，如果该模型有的话，每个模型会对应一个动画播放器
		* @param strName 模型在场景中的名称
		* @return CAnimationPlayer* 动画播放器指针
		*/
		CAnimationPlayer* _GetPlayerByModelName(const std::string& strModelName);
		/**
		* @brief 添加动画播放器
		* @param strPlayerName 播放器名称
		* @param pPlayer 动画播放器指针
		* @return bool 成功返回true，失败返回false
		*/
		bool _AddPlayer(const std::string& strPlayerName, CAnimationPlayer* pPlayer);
		/**
		* @brief 移除动画播放器
		* @param strPlayerName 播放器名称
		* @return bool 成功返回true，失败返回false
		*/
		bool _RemovePlayer(const std::string& strPlayerName);
		/**
		* @brief 清除所有动画播放器
		* @return bool 成功返回true，失败返回false
		*/
		bool _ClearPlayer();
		/**
		* @brief 获取动画播放器
		* @param strPlayerName 播放器名称
		* @return CAnimationPlayer* 动画播放器指针
		*/
		CAnimationPlayer* _GetPlayer(const std::string& strPlayerName);
		/**
		* @brief 获取播放器名称
		* @param strModelName 模型在场景中的名称
		* @return std::string 播放器名称
		*/
		std::string _GetPlayerName(const std::string& strModelName);

	private:
		//!< 动画播放器
		std::map<std::string, osg::ref_ptr<CAnimationPlayer>>	m_playerMap;
	};

}	// GM
