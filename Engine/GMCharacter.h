//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMCharacter.h
/// @brief		GMEngine - Character Module
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.07
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommon.h"
#include "GMKernel.h"

#include <vector>
#include <random>

namespace GM
{
	/*************************************************************************
	 Enums
	*************************************************************************/
	// 骨骼动画的枚举值
	enum EGMANIMATION_BONE
	{
		/** idle */
		EA_BONE_IDLE,
		/** 头朝左看 */
		EA_BONE_HEAD_L,
		/** 头朝右看 */
		EA_BONE_HEAD_R,
		/** 头朝上看 */
		EA_BONE_HEAD_U,
		/** 头朝下看 */
		EA_BONE_HEAD_D
	};
	// 变形动画的枚举值
	enum EGMANIMATION_MORPH
	{
		/** 眨眼 */
		EA_MORPH_BLINK,
		/** 口型“啊” */
		EA_MORPH_AA,
		/** 口型“哦” */
		EA_MORPH_OO
	};
	/*************************************************************************
	 Structs
	*************************************************************************/

	/*************************************************************************
	 Class
	*************************************************************************/

	/*!
	 *  @class CGMCharacter，系统单位：厘米
	 *  @brief GM Character Module
	 */
	class CGMCharacter
	{
	// 函数
	public:
		/** @brief 构造 */
		CGMCharacter();
		/** @brief 析构 */
		~CGMCharacter();

		/** @brief 初始化 */
		bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData);
		/** @brief 更新 */
		bool Update(double dDeltaTime);
		/** @brief 更新(在主相机更新姿态之后) */
		bool UpdatePost(double dDeltaTime);

		/**
		* @brief 激活角色的动画功能（骨骼动画、变形动画）
		* @param strName: 角色在场景中的名称
		* @param pNode: 角色的节点指针
		* @return bool 成功OK，失败Fail，角色不存在返回NotExist
		*/
		bool InitAnimation(const std::string& strName, osg::Node* pNode);
		/** @brief 传入眼睛的变幻节点的vector */
		void InitEyeTransform(std::vector<osg::ref_ptr<osg::Transform>>& v);

	private:
		void _InnerUpdate(const double dDeltaTime);
		/** @brief 定时更新眨眼状态 */
		void _InnerUpdateBlink(const double dDeltaTime);
		/** @brief 定时更新嘴唇状态 */
		void _InnerUpdateLip(const double dDeltaTime);
		/** @brief 改变注视方向 */
		void _ChangeLookDir(const double dDeltaTime);

		/** @brief 每帧更新转头动画的过渡状态 */
		void _UpdateLookAnimation(const double dDeltaTime);
		/** @brief 每帧更新眼球方向 */
		void _UpdateEye(const double dDeltaTime);

		/**
		* @brief 设置眼球相对于眼睛的方向
		* @param fHeading: 眼球偏航角，左正右负，单位：°
		* @param fPitch: 眼球俯仰角，上正下负，单位：°
		*/
		inline void _SetEyeFinalDir(float fHeading, float fPitch)
		{
			m_fEyeBallFinalHeading = osg::DegreesToRadians(fHeading);
			m_fEyeBallFinalPitch = osg::DegreesToRadians(fPitch);
		}

		/**
		* @brief 平滑过渡函数
		* @param fMin, fMax:			范围
		* @param fX:					映射前的值
		* @return double:				映射后的值[0.0, 1.0]
		*/
		inline double _Smoothstep(const double fMin, const double fMax, const double fX)
		{
			double y = osg::clampBetween((fX - fMin) / (fMax - fMin), 0.0, 1.0);
			return y * y * (3 - 2 * y);
		}

	// 变量
	private:
		SGMKernelData* m_pKernelData = nullptr;					//!< 内核数据
		SGMConfigData* m_pConfigData = nullptr;					//!< 配置数据

		std::default_random_engine m_iRandom;					//!< 随机值
		std::uniform_int_distribution<> m_iPseudoNoise;			//!< 伪随机数分布
		std::uniform_int_distribution<> m_iRandomAngle;			//!< 随机角度分布，[0,360]

		std::string m_strName = "";								//!< 角色名称
		float m_fLookDuration = 2.0f;							//!< 注视持续时间，单位：秒
		float m_fTurnDuration = 1.0f;							//!< 转头耗时，单位：秒
		float m_fMixTime = 1.0f;								//!< 当前混合所经过的时间，单位：秒
		float m_fHeading = 0.0f;								//!< 眼睛偏航角，左正右负，单位：°
		float m_fPitch = 0.0f;									//!< 眼睛俯仰角，上正下负，单位：°
		float m_fHeadingTargetWeight = 0;						//!< 转向动画的目标权重
		float m_fPitchTargetWeight = 0;							//!< 俯仰动画的目标权重
		float m_fHeadingSourceWeight = 0;						//!< 转向动画的起始权重
		float m_fPitchSourceWeight = 0;							//!< 俯仰动画的起始权重
		osg::Vec2 m_vHeadingWeight = osg::Vec2(0, 0);			//!< 2个转向动画的权重, x = left, y = right
		osg::Vec2 m_vPitchWeight = osg::Vec2(0, 0);				//!< 2个俯仰动画的权重, x= up, y = down

		EGMANIMATION_BONE m_eHeadingAnim = EA_BONE_HEAD_L;		//!< 转向动画
		EGMANIMATION_BONE m_eNextHeadingAnim = EA_BONE_HEAD_L;	//!< 下一个转向动画
		EGMANIMATION_BONE m_ePitchAnim = EA_BONE_HEAD_U;			//!< 俯仰动画
		EGMANIMATION_BONE m_eNextPitchAnim = EA_BONE_HEAD_U;		//!< 下一个俯仰动画

		std::vector<std::string> m_strBoneAnimNameVec;			//!< 骨骼动画名称vector
		std::vector<std::string> m_strMorphAnimNameVec;			//!< 变形动画名称vector

		std::vector<osg::ref_ptr<osg::Transform>> m_pEyeTransVector; //!< 眼球的变幻节点
		std::vector<osg::Matrix> m_mEyeTransVector;				//!< 眼球的变幻矩阵
		float m_fEyeBallFinalHeading = 0.0f;					//!< 眼球的最终偏航角，左正右负，单位：弧度
		float m_fEyeBallFinalPitch = 0.0f;						//!< 眼球俯最终仰角，上正下负，单位：弧度
		float m_fEyeBallHeading = 0.0f;							//!< 眼球当前偏航角，左正右负，单位：弧度
		float m_fEyeBallPitch = 0.0f;							//!< 眼球当前俯仰角，上正下负，单位：弧度
	};
}	// GM
