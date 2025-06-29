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
		/** 小动作：脚稍微动一动 */
		EA_BONE_IDLE_ADD_0,
		/** 跳舞0 */
		EA_BONE_DANCE_0,
		/** 跳舞1 */
		EA_BONE_DANCE_1,
		/** 头朝左看 */
		EA_BONE_HEAD_L,
		/** 头朝右看 */
		EA_BONE_HEAD_R,
		/** 头朝上看 */
		EA_BONE_HEAD_U,
		/** 头朝下看 */
		EA_BONE_HEAD_D,
		/** 左手抬起 */
		EA_BONE_ARM_L_UP,
		/** 右手抬起 */
		EA_BONE_ARM_R_UP,
	};
	// 变形动画的枚举值
	enum EGMANIMATION_MORPH
	{
		/** 眨眼 */
		EA_MORPH_BLINK,
		/** 半闭眼 */
		EA_MORPH_HALF,
		/** 口型的idle动画 */
		EA_MORPH_IDLE,
		/** 口型“啊” */
		EA_MORPH_AA,
		/** 口型“哦” */
		EA_MORPH_OO,
		/** 惊讶地睁大眼睛，并张嘴 */
		EA_MORPH_SURPRISE
	};
	/*************************************************************************
	 Structs
	*************************************************************************/
	/*!
	 *  @brief 动画数据结构体
	 */
	struct SGMAnimData
	{
		/** @brief 构造 */
		SGMAnimData(){}
		SGMAnimData(EGMANIMATION_BONE eAnim): eAnimation(eAnim){}
		/**
		* @brief 将权重设置得更接近目标
		* @param fDeltaTime 间隔时间，单位：秒
		* @param fSpeed 接近目标权重的速度比例，需要根据心情调整，[0.1, 5.0]
		* @return bool:	如果当前权重等于目标权重，则返回false，否则返回true
		*/
		bool SetWeightCloserToTarget(const float fDeltaTime, const float fSpeed)
		{
			if (fWeightNow == fWeightTarget) return false;

			// 后续会根据角色心情调整速度系数
			float fStep = osg::clampTo(fSpeed, 0.1f, 5.0f)*fDeltaTime*abs(fWeightTarget - fWeightSource);

			if (fWeightNow < fWeightTarget)
			{
				fWeightNow += fStep;
				fWeightNow = fmin(fWeightNow, fWeightTarget);
			}
			else
			{
				fWeightNow -= fStep;
				fWeightNow = fmax(fWeightNow, fWeightTarget);
			}
			return true;
		}
		/**
		* @brief 将权重在源权重和目标权重之间做差值混合
		* @param fMix 混合系数，参考glsl的mix函数
		* @return bool:	如果当前权重等于目标权重，则返回false，否则返回true
		*/
		inline bool SetWeightMix(const float fMix)
		{
			if (fWeightNow == fWeightTarget) return false;
			fWeightNow = fWeightSource * (1 - fMix) + fWeightTarget * fMix;
			return true;
		}

		float fWeightSource = 0;
		float fWeightNow = 0;
		float fWeightTarget = 0;
		EGMANIMATION_BONE eAnimation = EA_BONE_HEAD_L;
		bool bAnimOn = false;
	};

	/*!
	 *  @brief 用于存储“在第几个小节开始播放的某个舞蹈动作”的数据结构体
	 */
	struct SGMDanceSequence
	{
		SGMDanceSequence(int barStart, int barEnd, EGMANIMATION_BONE dance)
			: iBarStart(barStart), iBarEnd(barEnd), eDance(dance)
		{}

		// 在某个“小节”开始，0代表第一个“小节”（4个“拍子”组成1个“小节”）
		int iBarStart = 0;
		// 在某个“小节”结束，在这个小节内进行过渡操作
		int iBarEnd = 1;
		// 舞蹈动作的枚举值
		EGMANIMATION_BONE eDance = EA_BONE_DANCE_1;
	};

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

		/**
		* @brief 开启“欢迎效果”
		* @brief 每次开启软件，Engine调用此函数以实现“欢迎功能”
		*/
		void Welcome();

		/**
		* @brief 如果目标可见，则设置注视目标位置
		* @param vTargetWorldPos 目标点的世界空间坐标，单位：cm
		*/
		inline void SetLookTargetPos(const osg::Vec3d& vTargetWorldPos)
		{
			m_vTargetWorldPos = vTargetWorldPos;
		}
		/**
		* @brief 开启/关闭音频
		* @param bEnable：开启/关闭
		*/
		void SetMusicEnable(bool bEnable);
		inline bool GetMusicEnable() const { return m_bMusicOn; }
		/**
		* @brief 设置音频的总时长，单位：ms
		* @param iDuration: 音频的总时长
		*/
		void SetMusicDuration(int iDuration);
		/**
		* @brief 设置音频的播放位置，单位：ms
		* @param iTime: 音频的播放位置
		*/
		void SetMusicCurrentTime(int iTime);

	private:
		void _InnerUpdate(const double dDeltaTime);
		/** @brief 定时更新眨眼状态 */
		void _InnerUpdateBlink(const double dDeltaTime);
		/** @brief 定时更新嘴唇状态 */
		void _InnerUpdateLip(const double dDeltaTime);
		/** @brief 改变idle状态 */
		void _ChangeIdle(const double dDeltaTime);
		/** @brief 改变舞蹈动作 */
		void _ChangeDance(const double dDeltaTime);
		/** @brief 改变手部状态 */
		void _ChangeArm(const double dDeltaTime);
		/** @brief 改变注视方向 */
		void _ChangeLookDir(const double dDeltaTime);
		/** @brief 注视目标 */
		void _ChangeLookAtTarget(const double dDeltaTime);
		/** @brief 四处张望 */
		void _ChangeLookAround(const double dDeltaTime);
		/** @brief 改变目标动画 */
		void _ChangeTargetAnimation(const float fTargetHeading, const float fTargetPitch);

		/** @brief 每帧更新idle动画 */
		void _UpdateIdle(const double dDeltaTime);
		/** @brief 每帧更新舞蹈动作 */
		void _UpdateDance(const double dDeltaTime);
		/** @brief 每帧更新转头动画 */
		void _UpdateLookAnimation(const double dDeltaTime);
		/** @brief 每帧更新手部动画 */
		void _UpdateArmAnimation(const double dDeltaTime);

		/** @brief 每帧更新转头动画的过渡状态 */
		void _UpdateLookAt(const double dDeltaTime);
		/** @brief 每帧更新四处张望动画的过渡状态 */
		void _UpdateLookAround(const double dDeltaTime);
		/** @brief 每帧更新眼球方向 */
		void _UpdateEye(const double dDeltaTime);

		/**
		* @brief 将上下左右动画的权重设置得更接近目标
		* @param fDeltaTime 间隔时间，单位：秒
		* @param fSpeed 接近目标权重的速度比例，需要根据心情调整，[1,5]
		* @return bool:	如果上下左右动画的当前权重都等于目标权重，则返回false，否则返回true
		*/
		bool _SetHeadWeightToTarget(const float fDeltaTime, const float fSpeed);
		/**
		* @brief 将上下左右动画的权重在源权重和目标权重之间做差值混合
		* @param fMix 混合系数，参考glsl的mix函数
		* @return bool:	如果上下左右动画的当前权重都等于目标权重，则返回false，否则返回true
		*/
		bool _SetHeadWeightMix(const float fMix);

		void _UpdateHeadAnimation();

		void _StopAnimation(SGMAnimData& sAnim);

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

		float m_fIdleDuration = 5.0f;							//!< idle动画周期，单位：秒
		float m_fIdleAddDuration = 1.0f;						//!< idle附加动画周期，单位：秒
		float m_fIdleTime = 0.0f;								//!< idle动作经过了多长时间，单位：秒
		float m_fIdleAddTime = 0.0f;							//!< idle附加动作经过了多长时间，单位：秒
		float m_fArmTimeL = 0.0f;								//!< 左手动作经过了多长时间，单位：秒
		float m_fArmTimeR = 0.0f;								//!< 右手动作经过了多长时间，单位：秒
		float m_fArmDurationL = 4.0f;							//!< 左手动画周期，单位：秒
		float m_fArmDurationR = 4.0f;							//!< 右手动画周期，单位：秒

		float m_fLookDuration = 2.0f;							//!< 四处观望时，盯在某个方向的时间，单位：秒
		float m_fTurnDuration = 1.0f;							//!< 转头周期，单位：秒
		float m_fFastTurnDuration = 0.5f;						//!< 快速转头周期，单位：秒
		float m_fTurnMixTime = 0.0f;							//!< 当前转头动画混合所经过的时间，单位：秒

		float m_fTargetHeading = 0.0f;							//!< 眼睛偏航角，左正右负，单位：°
		float m_fTargetPitch = 0.0f;							//!< 眼睛俯仰角，上正下负，单位：°

		std::vector<SGMDanceSequence> m_danceSequenceVec;			//!< 存储舞蹈动作的顺序
		std::vector<SGMAnimData> m_animDanceVec;					//!< 舞蹈
		SGMAnimData m_animHeadL = SGMAnimData(EA_BONE_HEAD_L);		//!< left动作
		SGMAnimData m_animHeadR = SGMAnimData(EA_BONE_HEAD_R);		//!< right动作
		SGMAnimData m_animHeadU = SGMAnimData(EA_BONE_HEAD_U);		//!< up动作
		SGMAnimData m_animHeadD = SGMAnimData(EA_BONE_HEAD_D);		//!< down动作
		SGMAnimData m_animArmL = SGMAnimData(EA_BONE_ARM_L_UP);		//!< 左手举起动作
		SGMAnimData m_animArmR = SGMAnimData(EA_BONE_ARM_R_UP);		//!< 右手举起动作

		EGMANIMATION_BONE m_eNextHeadingAnim = EA_BONE_HEAD_L;	//!< 下一个转向动画
		EGMANIMATION_BONE m_eNextPitchAnim = EA_BONE_HEAD_U;	//!< 下一个俯仰动画
		EGMANIMATION_BONE m_eLastDanceAnim = EA_BONE_IDLE;		//!< 上一个舞蹈动画

		std::vector<std::string> m_strBoneAnimNameVec;			//!< 骨骼动画名称vector
		std::vector<std::string> m_strMorphAnimNameVec;			//!< 变形动画名称vector

		std::vector<osg::ref_ptr<osg::Transform>> m_pEyeTransVector; //!< 眼球的变幻节点
		std::vector<osg::Matrix> m_mEyeTransVector;				//!< 眼球的变幻矩阵
		float m_fEyeBallFinalHeading = 0.0f;					//!< 眼球的最终偏航角，左正右负，单位：弧度
		float m_fEyeBallFinalPitch = 0.0f;						//!< 眼球俯最终仰角，上正下负，单位：弧度
		float m_fEyeBallHeading = 0.0f;							//!< 眼球当前偏航角，左正右负，单位：弧度
		float m_fEyeBallPitch = 0.0f;							//!< 眼球当前俯仰角，上正下负，单位：弧度

		//!< 好奇
		//! [0.0, 1.0]
		//! 0.0 == 清心寡欲，无视目标；1.0 == 强烈的好奇心，会完全被目标吸引
		float m_fInterest = 0.0f;
		//!< 开心
		//! [0.0, 1.0]
		//! 0.0 == 悲痛欲绝；0.5 == 平静; 1.0 == 欣喜若狂
		float m_fHappy = 0.5f;
		//!< 愤怒
		//! [0.0, 1.0]
		//! 0.0 == 平静；1.0 == 非常愤怒
		float m_fAngry = 0.0f;
		//!< 害怕
		//! [0.0, 1.0]
		//! 0.0 == 平静；1.0 == 感到恐怖
		float m_fScared = 0.0f;

		bool m_bDisdain = false;								//!< 是否在鄙视（以后考虑加入性格）
		bool m_bMusicOn = false;								//!< 音乐是否开启
		bool m_bStartDance = true;								//!< 是否刚开始舞蹈
		bool m_bReStartDance = false;							//!< 是否被打断舞蹈，需要重新开始舞蹈

		float m_fMusicTime = 0.0f;								//!< 音乐播放的时间，单位：秒
		float m_fMusicBeatTime = 0.5f;							//!< 音乐的拍子，单位：秒
		float m_fMusicBarTime = 2.0f;							//!< 音乐的小节（4拍子），单位：秒
		float m_fMusicPhraseTime = 8.0f;						//!< 音乐的乐段（4小节），单位：秒
		float m_fMusicDuration = 1.0f;							//!< 音乐的总时长，单位：秒
		int m_iBarCount = -1;									//!< 小节编号，从0开始，默认-1

		float m_fDeltaVelocity = 0;								//!< 目标点的速度差，单位：cm/s
		osg::Vec3d m_vTargetWorldPos = osg::Vec3d(0,-30,0);		//!< 目标点的世界空间坐标，单位：cm
		osg::Vec3d m_vTargetLastWorldPos = osg::Vec3d(0, -30, 0);//!< 目标点上一次指定的世界空间坐标，单位：cm
		osg::Vec3d m_vTargetLastVelocity = osg::Vec3d(0, 0, 0);	//!< 目标点上一次的速度，单位：cm/s
	};
}	// GM
