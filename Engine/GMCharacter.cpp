//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMCharacter.cpp
/// @brief		GMEngine - Character Module
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.07
//////////////////////////////////////////////////////////////////////////
#include "GMCharacter.h"
#include "GMCommonUniform.h"
#include "GMKit.h"
#include "Animation/GMAnimation.h"
#include <osg/MatrixTransform>

using namespace GM;

/*************************************************************************
Global Constants
*************************************************************************/

#define  BONE_PRIORITY_HIGHEST				(13)		// 骨骼动画“最高”优先级
#define  BONE_PRIORITY_HIGH					(12)		// 骨骼动画“高”优先级
#define  BONE_PRIORITY_NORMAL				(11)		// 骨骼动画“普通”优先级
#define  BONE_PRIORITY_LOW					(10)		// 骨骼动画“低”优先级

#define  MORPH_PRIORITY_HIGHEST				(3)			// 变形动画“最高”优先级
#define  MORPH_PRIORITY_HIGH				(2)			// 变形动画“高”优先级
#define  MORPH_PRIORITY_NORMAL				(1)			// 变形动画“普通”优先级

#define  IDLE_ADD_FADE_TIME					(0.5f)		// idle附加动画的淡入淡出时间，单位：秒
#define  ARM_FADE_TIME						(0.8f)		// 手部动画的淡入淡出时间，单位：秒

/*************************************************************************
CGMCharacter Methods
*************************************************************************/
/** @brief 构造 */
CGMCharacter::CGMCharacter()
{
	m_iRandom.seed(0);
	m_iPseudoNoise = std::uniform_int_distribution<>(0, 100);
	m_iRandomAngle = std::uniform_int_distribution<>(0, 360);

	m_strBoneAnimNameVec.push_back("bone_idle");
	m_strBoneAnimNameVec.push_back("bone_idleAdd_0");
	m_strBoneAnimNameVec.push_back("bone_dance_0");
	m_strBoneAnimNameVec.push_back("bone_dance_1");
	m_strBoneAnimNameVec.push_back("bone_head_L");
	m_strBoneAnimNameVec.push_back("bone_head_R");
	m_strBoneAnimNameVec.push_back("bone_head_U");
	m_strBoneAnimNameVec.push_back("bone_head_D");
	m_strBoneAnimNameVec.push_back("bone_arm_L_up");
	m_strBoneAnimNameVec.push_back("bone_arm_R_up");

	m_strMorphAnimNameVec.push_back("eye_blink");
	m_strMorphAnimNameVec.push_back("eye_half");
	m_strMorphAnimNameVec.push_back("mouth_idle");
	m_strMorphAnimNameVec.push_back("mouth_aa");
	m_strMorphAnimNameVec.push_back("mouth_oo");
	m_strMorphAnimNameVec.push_back("all_surprise");

	m_animDanceVec.push_back(SGMAnimData(EA_BONE_DANCE_0));
	m_animDanceVec.push_back(SGMAnimData(EA_BONE_DANCE_1));
}

/** @brief 析构 */
CGMCharacter::~CGMCharacter()
{
}

/** @brief 初始化 */
bool CGMCharacter::Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData)
{
	m_pKernelData = pKernelData;
	m_pConfigData = pConfigData;

	m_danceSequenceVec.push_back(SGMDanceSequence(14, 16, EA_BONE_DANCE_1));
	m_danceSequenceVec.push_back(SGMDanceSequence(39, 42, EA_BONE_DANCE_1));
	m_danceSequenceVec.push_back(SGMDanceSequence(65, 67, EA_BONE_DANCE_1));
	m_danceSequenceVec.push_back(SGMDanceSequence(84, 86, EA_BONE_DANCE_1));

	return true;
}

bool CGMCharacter::Update(double dDeltaTime)
{
	// 程序开始的时候，角色必须忽视目标一段时间
	m_fSyncTime += dDeltaTime;
	// 刚鄙视完，气还没消，直接无视目标
	m_bLookAtTarget = (m_fSyncTime > 5) && (!m_bDisdain || (m_fAngry < 0.5)) && (m_fInterest > 0.2);

	static double s_fConstantStep = 0.05;
	static double s_fDeltaStep = 0.0;
	if (s_fDeltaStep > s_fConstantStep)
	{
		_InnerUpdate(s_fDeltaStep);
		s_fDeltaStep = 0.0;
	}
	s_fDeltaStep += dDeltaTime;

	// 更新idle动画权重
	_UpdateIdle(dDeltaTime);
	// 更新舞蹈动画权重
	_UpdateDance(dDeltaTime);
	// 过渡状态时需要每帧更新转头动画的权重
	_UpdateLookAnimation(dDeltaTime);
	// 更新手部动画权重
	_UpdateArmAnimation(dDeltaTime);

	// 每帧累加音乐播放时间
	if(m_bMusicOn) m_fMusicTime += dDeltaTime;

	// 每帧都在减少好奇心（注意力很难长时间集中）
	m_fInterest = fmax(0.0f, m_fInterest - 0.5f * dDeltaTime);
	// 每帧都在趋于平静（心情会慢慢变好）
	m_fHappy = CGMKit::Mix(m_fHappy, 0.5f, min(1.0f, 0.05f * float(dDeltaTime)));
	// 每帧都在减少愤怒值（消气）
	m_fAngry = fmax(0.0f, m_fAngry - 0.03f * dDeltaTime);
	// 每帧都在减少害怕值
	m_fScared = fmax(0.0f, m_fScared - 0.4f * dDeltaTime);

	return true;
}

bool CGMCharacter::UpdatePost(double dDeltaTime)
{
	// 必须在更新过骨骼之后再更新眼球
	_UpdateEye(dDeltaTime);
	return true;
}

bool CGMCharacter::InitAnimation(const std::string& strName, osg::Node* pNode)
{
	if (!pNode) return false;
	m_strName = strName;

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_LOOP, m_strBoneAnimNameVec.at(EA_BONE_IDLE));
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_LOW, m_strBoneAnimNameVec.at(EA_BONE_IDLE));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strBoneAnimNameVec.at(EA_BONE_IDLE_ADD_0));
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_NORMAL, m_strBoneAnimNameVec.at(EA_BONE_IDLE_ADD_0));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_LOOP, m_strBoneAnimNameVec.at(EA_BONE_DANCE_0));
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_NORMAL, m_strBoneAnimNameVec.at(EA_BONE_DANCE_0));
	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_LOOP, m_strBoneAnimNameVec.at(EA_BONE_DANCE_1));
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_NORMAL, m_strBoneAnimNameVec.at(EA_BONE_DANCE_1));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_LOOP, m_strBoneAnimNameVec.at(EA_BONE_HEAD_L));
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_HIGH, m_strBoneAnimNameVec.at(EA_BONE_HEAD_L));
	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_LOOP, m_strBoneAnimNameVec.at(EA_BONE_HEAD_R));
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_HIGH, m_strBoneAnimNameVec.at(EA_BONE_HEAD_R));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_LOOP, m_strBoneAnimNameVec.at(EA_BONE_HEAD_U));
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_HIGH, m_strBoneAnimNameVec.at(EA_BONE_HEAD_U));
	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_LOOP, m_strBoneAnimNameVec.at(EA_BONE_HEAD_D));
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_HIGH, m_strBoneAnimNameVec.at(EA_BONE_HEAD_D));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strBoneAnimNameVec.at(EA_BONE_ARM_L_UP));
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_HIGHEST, m_strBoneAnimNameVec.at(EA_BONE_ARM_L_UP));
	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strBoneAnimNameVec.at(EA_BONE_ARM_R_UP));
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_HIGHEST, m_strBoneAnimNameVec.at(EA_BONE_ARM_R_UP));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strMorphAnimNameVec.at(EA_MORPH_BLINK));
	GM_ANIMATION.SetAnimationPriority(strName, MORPH_PRIORITY_NORMAL, m_strMorphAnimNameVec.at(EA_MORPH_BLINK));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strMorphAnimNameVec.at(EA_MORPH_HALF));
	GM_ANIMATION.SetAnimationPriority(strName, MORPH_PRIORITY_HIGH, m_strMorphAnimNameVec.at(EA_MORPH_HALF));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strMorphAnimNameVec.at(EA_MORPH_IDLE));
	GM_ANIMATION.SetAnimationPriority(strName, MORPH_PRIORITY_NORMAL, m_strMorphAnimNameVec.at(EA_MORPH_IDLE));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strMorphAnimNameVec.at(EA_MORPH_AA));
	GM_ANIMATION.SetAnimationPriority(strName, MORPH_PRIORITY_HIGH, m_strMorphAnimNameVec.at(EA_MORPH_AA));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strMorphAnimNameVec.at(EA_MORPH_OO));
	GM_ANIMATION.SetAnimationPriority(strName, MORPH_PRIORITY_HIGH, m_strMorphAnimNameVec.at(EA_MORPH_OO));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strMorphAnimNameVec.at(EA_MORPH_SURPRISE));
	GM_ANIMATION.SetAnimationPriority(strName, MORPH_PRIORITY_HIGH, m_strMorphAnimNameVec.at(EA_MORPH_SURPRISE));

	GM_ANIMATION.SetAnimationPlay(strName, m_strBoneAnimNameVec.at(EA_BONE_IDLE));
	GM_ANIMATION.SetAnimationWeight(strName, 1.0, m_strBoneAnimNameVec.at(EA_BONE_IDLE));

	return true;
}

void CGMCharacter::InitEyeTransform(std::vector<osg::ref_ptr<osg::Transform>>& v)
{
	m_pEyeTransVector = v;
	for (auto& itr : m_pEyeTransVector)
	{
		m_mEyeTransVector.push_back(itr->asMatrixTransform()->getMatrix());
	}
}

void CGMCharacter::Welcome()
{
}

void CGMCharacter::SetMusicEnable(bool bEnable)
{
	if (m_bMusicOn == bEnable) return;

	m_bMusicOn = bEnable;
	m_fMusicTime = 0.0f;
	m_iBarCount = -1;
	m_eLastDanceAnim = EA_BONE_IDLE;
	// 立刻开始四处看
	m_fLookDuration = 0;

	// “惊讶”
	if (m_bMusicOn)
	{
		m_fInterest = 1.0f;
		if (!GM_ANIMATION.IsAnimationPlaying(m_strName, m_strMorphAnimNameVec.at(EA_MORPH_SURPRISE)))
		{
			GM_ANIMATION.SetAnimationDuration(m_strName, 6.0f, m_strMorphAnimNameVec.at(EA_MORPH_SURPRISE));
			GM_ANIMATION.SetAnimationWeight(m_strName, 1.0, m_strMorphAnimNameVec.at(EA_MORPH_SURPRISE));
			GM_ANIMATION.SetAnimationPlay(m_strName, m_strMorphAnimNameVec.at(EA_MORPH_SURPRISE));
		}
	}
	else
	{
		for (auto& itr : m_animDanceVec)
		{
			if (!itr.bAnimOn) continue;

			itr.fWeightSource = itr.fWeightNow;
			itr.fWeightTarget = 0;
		}
	}
}

void CGMCharacter::SetMusicDuration(int iDuration)
{
	m_fMusicDuration = iDuration / 1000.0f;
}

void CGMCharacter::SetMusicCurrentTime(int iTime)
{
	m_fMusicTime = iTime / 1000.0f;
	m_eLastDanceAnim = EA_BONE_IDLE;
	m_bReStartDance = true;
	for (auto& itr : m_animDanceVec)
	{
		if (!itr.bAnimOn) continue;

		itr.fWeightSource = itr.fWeightNow;
		itr.fWeightTarget = 0;
	}

	// 立刻向下看，这种情况就相当于目标突然出现又突然消失，则开始搜寻目标，并计时
	m_vTargetWorldPos = osg::Vec3d(20 * (m_fMusicTime - 0.3f*m_fMusicDuration) / m_fMusicDuration, -20, -20);
	// 重置上一帧的注视目标位置
	m_vTargetLastWorldPos = m_vTargetWorldPos;
}

void CGMCharacter::_InnerUpdate(const double dDeltaTime)
{
	// 计算目标点的加速度
	osg::Vec3d vTargetVelocity = (m_vTargetWorldPos - m_vTargetLastWorldPos) / dDeltaTime;
	osg::Vec3d vDeltaVelocity = vTargetVelocity - m_vTargetLastVelocity;
	m_fDeltaVelocity = vDeltaVelocity.length();

	// 更新兴趣值
	m_fInterest = min(1.0f, m_fInterest + 0.002f * float(m_fDeltaVelocity));

	if (m_bMusicOn)
	{
		m_fHappy = min(1.0f, m_fHappy + 0.01f*dDeltaTime);
		_ChangeDance(dDeltaTime);
	}
	else
	{
		_ChangeIdle(dDeltaTime);
	}

	// 改变手部动画
	_ChangeArm(dDeltaTime);
	// 眼睛转向
	_ChangeLookDir(dDeltaTime);
	// 眨眼
	_InnerUpdateBlink(dDeltaTime);
	// 嘴唇
	_InnerUpdateLip(dDeltaTime);

	//if (m_fInterest > 0 || m_fAngry > 0 || m_fScared > 0 )
	//{
	//	std::cout << "Interest: " << m_fInterest << "   Angry:" << m_fAngry <<"   Scared: " << m_fScared << std::endl;
	//}
	//if (m_fInterest > 0)
	//{
	//	std::cout << "Interest: " << m_fInterest << std::endl;
	//}
	//if (m_fDeltaVelocity > 0)
	//{
	//	std::cout << "DeltaVelocity: " << m_fDeltaVelocity << std::endl;
	//}

	m_vTargetLastVelocity = vTargetVelocity;
	m_vTargetLastWorldPos = m_vTargetWorldPos;
}

void CGMCharacter::_InnerUpdateBlink(const double dDeltaTime)
{
	static double s_fBlinkTime = 0.0;
	static double s_fDeltaBlinkTime = 2.0;
	// “惊讶”和“半闭眼”时不眨眼
	if (s_fBlinkTime > s_fDeltaBlinkTime)
	{
		GM_ANIMATION.SetAnimationWeight(m_strName, 1.0, m_strMorphAnimNameVec.at(EA_MORPH_BLINK));
		GM_ANIMATION.SetAnimationPlay(m_strName, m_strMorphAnimNameVec.at(EA_MORPH_BLINK));
		s_fBlinkTime = 0.0;
		s_fDeltaBlinkTime = m_iPseudoNoise(m_iRandom) * 0.1 + 0.3;
	}
	s_fBlinkTime += dDeltaTime;
}

void CGMCharacter::_InnerUpdateLip(const double dDeltaTime)
{
	static double s_fAATime = 0.0;
	static double s_fDeltaAATime = 2.0;
	// “惊讶”和“半闭眼”时不会做其他口型
	if (s_fAATime > s_fDeltaAATime && m_fAngry < 0.9)
	{
		float fMorphDuration = m_iPseudoNoise(m_iRandom) * 0.05 + 2;
		GM_ANIMATION.SetAnimationDuration(m_strName, fMorphDuration, m_strMorphAnimNameVec.at(EA_MORPH_IDLE));
		GM_ANIMATION.SetAnimationWeight(m_strName, 1.0, m_strMorphAnimNameVec.at(EA_MORPH_IDLE));
		GM_ANIMATION.SetAnimationPlay(m_strName, m_strMorphAnimNameVec.at(EA_MORPH_IDLE));

		s_fAATime = 0.0;
		s_fDeltaAATime = m_iPseudoNoise(m_iRandom) * 0.1 + fMorphDuration;
	}
	s_fAATime += dDeltaTime;
}

void CGMCharacter::_ChangeIdle(const double dDeltaTime)
{
	if( (m_fIdleTime >= (m_fIdleDuration + m_fIdleAddDuration)) && (0.0 == m_fIdleAddTime) )
	{
		m_fIdleTime = 0.0;
		// 开始在“Idle动画”中混入“转身动画”，也就是让转身动画时间不为0
		m_fIdleAddTime += 1e-5;
		m_fIdleDuration = (m_iPseudoNoise(m_iRandom) % 4 + 1) * 4.0;
		m_fIdleAddDuration = m_iPseudoNoise(m_iRandom) * 0.07 + 3.0;
		GM_ANIMATION.SetAnimationDuration(m_strName, m_fIdleAddDuration, m_strBoneAnimNameVec.at(EA_BONE_IDLE_ADD_0));
		GM_ANIMATION.SetAnimationWeight(m_strName, 0.0, m_strBoneAnimNameVec.at(EA_BONE_IDLE_ADD_0));
		GM_ANIMATION.SetAnimationPlay(m_strName, m_strBoneAnimNameVec.at(EA_BONE_IDLE_ADD_0));
	}
}

void CGMCharacter::_ChangeDance(const double dDeltaTime)
{
	if (m_fHappy > 0.5f)
	{
		EGMANIMATION_BONE eDanceAnim = EA_BONE_IDLE;
		// 计算当前的乐段编号
		int iBarCount = int(m_fMusicTime / m_fMusicBarTime);
		if (m_iBarCount != iBarCount)
		{
			bool bChangeDance = false;
			m_iBarCount = iBarCount;
			eDanceAnim = EA_BONE_DANCE_0;
			for (auto& itr : m_danceSequenceVec)
			{
				if (itr.iBarStart <= iBarCount && iBarCount < itr.iBarEnd)
				{
					eDanceAnim = itr.eDance;
					break;
				}
			}

			for (auto& itr : m_animDanceVec)
			{
				if (m_eLastDanceAnim != eDanceAnim)
				{
					bChangeDance = true;
					if (eDanceAnim == itr.eAnimation)
					{
						itr.bAnimOn = true;
						itr.fWeightSource = 0;
						itr.fWeightNow = 0.0001;
						itr.fWeightTarget = 1;
						GM_ANIMATION.SetAnimationWeight(m_strName, 0.0001, m_strBoneAnimNameVec.at(itr.eAnimation));
						GM_ANIMATION.SetAnimationPlay(m_strName, m_strBoneAnimNameVec.at(itr.eAnimation));
					}
					else
					{
						// 一旦有新的舞蹈动画播放，则停止当前的舞蹈动画，
						if (itr.bAnimOn)
						{
							itr.fWeightSource = itr.fWeightNow;
							itr.fWeightTarget = 0;
						}
					}
				}

				// 一旦有舞蹈动画权重为1，则将“初次跳舞”标记为false
				if (1.0f == itr.fWeightNow)
					m_bStartDance = false;
			}
			if (bChangeDance)
			{
				m_eLastDanceAnim = eDanceAnim;
			}
		}
	}
}

void CGMCharacter::_ChangeArm(const double dDeltaTime)
{
	static float s_fWaveSumTime = 0.0f;	// 目标挥舞的时间，单位：秒
	static float s_fArmUpAcceThreshold = 0.05f;	// 让手部抬起的目标挥舞加速度阈值，单位：cm/s

	// 目标加速度大于某个阈值时，才会抬手，
	if(m_fDeltaVelocity > s_fArmUpAcceThreshold)
		s_fWaveSumTime += dDeltaTime;
	else
		s_fWaveSumTime = 0.0f;
	// 目标挥舞的时间大于1秒，才会抬手
	bool bArmUp = s_fWaveSumTime > 1.0f;
	if (bArmUp)
		s_fWaveSumTime = 0.0f;


	// 加速度越大，抬手越快
	float fArmDuration = 6.0f / osg::clampTo((m_fDeltaVelocity - s_fArmUpAcceThreshold) * 0.2, 1.0, 2.0);
	// 抬手不能过于频繁，所以等一段时间后，下一个手臂动画才会开始
	if (bArmUp && (m_fArmTimeL > m_fArmDurationL) && (m_fInterest > 0.5)
		&& (m_animHeadL.fWeightNow > 0.1) && (m_animHeadL.fWeightTarget > 0.2))
	{
		// 重置手部动画的时间
		m_fArmTimeL = 0.0;
		m_animArmL.bAnimOn = true;
		m_animArmL.fWeightSource = 0;
		m_animArmL.fWeightTarget = max(m_animHeadL.fWeightNow, m_animHeadU.fWeightNow);
		m_fArmDurationL = fArmDuration;
		
		GM_ANIMATION.SetAnimationDuration(m_strName, m_fArmDurationL, m_strBoneAnimNameVec.at(EA_BONE_ARM_L_UP));
		GM_ANIMATION.SetAnimationPlay(m_strName, m_strBoneAnimNameVec.at(EA_BONE_ARM_L_UP));
	}
	else
	{
		// 如果头已经看向右边，左手动画就不需要了
		if ((m_animHeadR.fWeightTarget > 0.0) && (m_fArmDurationL > (m_fArmTimeL + ARM_FADE_TIME)))
		{
			m_fArmDurationL = m_fArmTimeL + ARM_FADE_TIME;
		}
	}

	if (bArmUp && (m_fArmTimeR > m_fArmDurationR) && (m_fInterest > 0.5)
		&& (m_animHeadR.fWeightNow > 0.1) && (m_animHeadR.fWeightTarget > 0.2))
	{
		// 重置手部动画的时间
		m_fArmTimeR = 0.0;
		m_animArmR.bAnimOn = true;
		m_animArmR.fWeightSource = 0;
		m_animArmR.fWeightTarget = max(m_animHeadR.fWeightNow, m_animHeadU.fWeightNow);
		m_fArmDurationR = fArmDuration;

		GM_ANIMATION.SetAnimationDuration(m_strName, m_fArmDurationR, m_strBoneAnimNameVec.at(EA_BONE_ARM_R_UP));
		GM_ANIMATION.SetAnimationPlay(m_strName, m_strBoneAnimNameVec.at(EA_BONE_ARM_R_UP));
	}
	else
	{
		// 如果头已经看向左边，右手部动画就不需要了
		if ((m_animHeadL.fWeightTarget > 0.0) && (m_fArmDurationR > (m_fArmTimeR + ARM_FADE_TIME)))
		{
			m_fArmDurationR = m_fArmTimeR + ARM_FADE_TIME;
		}
	}
}

void CGMCharacter::_ChangeLookDir(const double dDeltaTime)
{
	// 角色注视目标点一段时间
	if (m_bLookAtTarget)
	{
		_ChangeLookAtTarget(dDeltaTime);
	}
	else// 否则忽视目标，则执行“四处张望”的功能
	{
		_ChangeLookAround(dDeltaTime);
	}
}

void CGMCharacter::_ChangeLookAtTarget(const double dDeltaTime)
{
	if (m_bDisdain)
	{
		if (m_fAngry < 0.5)
		{
			m_bDisdain = false;
		}
	}
	else
	{
		// 如果目标在变速运动，且愤怒值小于某个阈值，则角色的好奇开始增加
		if (m_fAngry < 0.7)
		{
			m_fInterest = fmin(1.0f, m_fInterest + m_fDeltaVelocity * 3e-3f);
		}

		// 如果变化不剧烈，则不会增加愤怒，超过一定的速度后才会增加
		float fAddAngry = fmax(0.0f, m_fDeltaVelocity - 20.0f);
		// 如果短时间内加速度模的累计值过大，说明目标在剧烈运动，可以认为在耍自己，愤怒值飙升
		m_fAngry = fmin(m_fAngry + fAddAngry * fAddAngry * 2e-7f, 1.0f);

		// 对短时加速度过大的物体会有危机感，增加害怕值
		m_fScared = fmin(m_fScared + fmax(0.0f, m_fDeltaVelocity - 10.0f) * 5e-4f, 1.0f);
	}

	osg::Vec3d vDir = osg::Vec3d(0, -1, 0);
	// 愤怒值超过一个阈值后，角色会放弃注视目标并看向前方一段时间，这时会减少愤怒值
	// 愤怒值小于一个阈值后才会继续随意看四周，此时愤怒值会继续减小一直到0，然后才能继续注视
	if (m_fAngry > 0.9)
	{
		if (!m_bDisdain)
		{
			m_bDisdain = true;
			GM_ANIMATION.SetAnimationPlay(m_strName, m_strMorphAnimNameVec.at(EA_MORPH_HALF));
			GM_ANIMATION.SetAnimationWeight(m_strName, 1.0, m_strMorphAnimNameVec.at(EA_MORPH_HALF));
			GM_ANIMATION.SetAnimationDuration(m_strName, 4.0f, m_strMorphAnimNameVec.at(EA_MORPH_HALF));
		}
	}
	else if(m_fAngry < 0.55)
	{
		vDir = m_vTargetWorldPos - osg::Vec3d(0.0, -1.0, 12.0);
		// 保证角色不会看向后方
		vDir.y() = std::fmin(vDir.y(), 0.0);
		vDir.normalize();

		_SetEyeFinalDir(0, 0);
	}
	else
	{
		_SetEyeFinalDir(0, 0);
	}

	float fTargetHeading = osg::RadiansToDegrees(std::atan2(-vDir.x(), -vDir.y()));
	float fTargetPitch = osg::RadiansToDegrees(std::asin(vDir.z()));
	_ChangeTargetAnimation(fTargetHeading, fTargetPitch);
}

void CGMCharacter::_ChangeLookAround(const double dDeltaTime)
{
	static double s_fLookAroundTime = 0.0;
	if (s_fLookAroundTime > m_fTurnDuration) // 朝向固定的时候
	{
		static int s_i = 0;
		if (s_i > 20)
		{
			_SetEyeFinalDir(
				m_iRandomAngle(m_iRandom) / 36.0 - 5.0, // [-5, 5]
				m_iRandomAngle(m_iRandom) / 24.0 - 7.5); // [-7.5, 7.5]
			s_i = 0;
		}
		s_i++;
	}

	if (s_fLookAroundTime >= (m_fLookDuration + m_fTurnDuration)) // 开始改变朝向
	{
		float fDeltaHeading = m_iRandomAngle(m_iRandom) / 3.0 - 60.0; // [-60, 60]
		float fDeltaPitch = m_iRandomAngle(m_iRandom) / 12.0 - 15.0; // [-15, 15]

		// 下一个偏转角，[-90, 90]
		float fHeading = m_fTargetHeading + fDeltaHeading;
		if (fHeading < -90) fHeading += 90;
		else if (fHeading > 90) fHeading -= 90;
		else {}

		// 下一个俯仰角，[-30, 30]
		float fPitch = m_fTargetPitch + fDeltaPitch;
		if (fPitch < -30) fPitch += 30;
		else if (fPitch > 30) fPitch -= 30;
		else {}

		// 需要让眼睛大部分时间都朝前看，所以给俯仰角和偏转角乘了个系数
		float fCenterCloser = m_iPseudoNoise(m_iRandom) * 0.01; // [0,1]
		fHeading *= fCenterCloser;
		fPitch *= fCenterCloser;
		
		// 计算当前朝向与下一个朝向的差值，以便于设置转向时间
		fDeltaHeading = fHeading - m_fTargetHeading;
		fDeltaPitch = fPitch - m_fTargetPitch;
		float fDeltaAngle = abs(fDeltaHeading) + abs(fDeltaPitch);
		// 眼球先动，眼睛再跟着专向
		_SetEyeFinalDir(osg::clampTo(fDeltaHeading, -10.0f, 10.0f), osg::clampTo(fDeltaPitch, -20.0f, 20.0f));

		// 头部动画
		m_animHeadL.fWeightSource = m_animHeadL.fWeightTarget;
		m_animHeadR.fWeightSource = m_animHeadR.fWeightTarget;
		m_animHeadU.fWeightSource = m_animHeadU.fWeightTarget;
		m_animHeadD.fWeightSource = m_animHeadD.fWeightTarget;

		// 这里要保证传入的是最终的俯仰和偏航角
		_ChangeTargetAnimation(fHeading, fPitch);

		s_fLookAroundTime = 0.0;
		//重置混合时间，开始混合动画
		m_fTurnMixTime = 0.0f;
		m_fLookDuration = max(0, m_iPseudoNoise(m_iRandom) * 0.001 / max(0.05, 0.2*m_fInterest)) + 0.5;
		m_fTurnDuration = ((m_iPseudoNoise(m_iRandom) * 0.00001 + 0.001) * fDeltaAngle + 0.03) / max(0.1, 0.15*m_fInterest);
	}
	s_fLookAroundTime += dDeltaTime;
}

void CGMCharacter::_ChangeTargetAnimation(const float fTargetHeading, const float fTargetPitch)
{
	m_fTargetHeading = fTargetHeading;
	m_fTargetPitch = fTargetPitch;
	if (m_fTargetHeading < 0) //需要朝左
	{
		m_animHeadL.fWeightSource = m_animHeadL.fWeightNow;
		m_animHeadL.fWeightTarget = abs(m_fTargetHeading) / 90;
		m_animHeadL.bAnimOn = true;

		m_animHeadR.fWeightSource = m_animHeadR.fWeightNow;
		m_animHeadR.fWeightTarget = 0;

		m_eNextHeadingAnim = EA_BONE_HEAD_L;
	}
	else //需要朝右
	{
		m_animHeadL.fWeightSource = m_animHeadL.fWeightNow;
		m_animHeadL.fWeightTarget = 0;

		m_animHeadR.fWeightSource = m_animHeadR.fWeightNow;
		m_animHeadR.fWeightTarget = abs(m_fTargetHeading) / 90;
		m_animHeadR.bAnimOn = true;

		m_eNextHeadingAnim = EA_BONE_HEAD_R;
	}

	if (m_fTargetPitch < 0) //需要朝下
	{
		m_animHeadU.fWeightSource = m_animHeadU.fWeightNow;
		m_animHeadU.fWeightTarget = 0;

		m_animHeadD.fWeightSource = m_animHeadD.fWeightNow;
		m_animHeadD.fWeightTarget = abs(m_fTargetPitch) / 90;
		m_animHeadD.bAnimOn = true;

		m_eNextPitchAnim = EA_BONE_HEAD_D;
	}
	else //需要朝上
	{
		m_animHeadU.fWeightSource = m_animHeadU.fWeightNow;
		m_animHeadU.fWeightTarget = abs(m_fTargetPitch) / 90;
		m_animHeadU.bAnimOn = true;

		m_animHeadD.fWeightSource = m_animHeadD.fWeightNow;
		m_animHeadD.fWeightTarget = 0;

		m_eNextPitchAnim = EA_BONE_HEAD_U;
	}

	if (!GM_ANIMATION.IsAnimationPlaying(m_strName, m_strBoneAnimNameVec.at(m_eNextHeadingAnim)))
		GM_ANIMATION.SetAnimationPlay(m_strName, m_strBoneAnimNameVec.at(m_eNextHeadingAnim));
	if (!GM_ANIMATION.IsAnimationPlaying(m_strName, m_strBoneAnimNameVec.at(m_eNextPitchAnim)))
		GM_ANIMATION.SetAnimationPlay(m_strName, m_strBoneAnimNameVec.at(m_eNextPitchAnim));
}

void CGMCharacter::_UpdateIdle(const double dDeltaTime)
{
	if (0 < m_fIdleAddTime)
	{
		if (IDLE_ADD_FADE_TIME >= m_fIdleAddTime)
		{
			// “Idle附加动画”淡入
			float fIdleAddWeight = _Smoothstep(0.0f, IDLE_ADD_FADE_TIME, m_fIdleAddTime);
			GM_ANIMATION.SetAnimationWeight(m_strName, fIdleAddWeight, m_strBoneAnimNameVec.at(EA_BONE_IDLE_ADD_0));
		}
		else if ((m_fIdleAddDuration - IDLE_ADD_FADE_TIME) <= m_fIdleAddTime)
		{
			// “Idle附加动画”淡出
			float fTurnWeight = _Smoothstep(0.0f, IDLE_ADD_FADE_TIME, m_fIdleAddDuration - m_fIdleAddTime);
			GM_ANIMATION.SetAnimationWeight(m_strName, fTurnWeight, m_strBoneAnimNameVec.at(EA_BONE_IDLE_ADD_0));
		}
		else
		{
			GM_ANIMATION.SetAnimationWeight(m_strName, 1.0, m_strBoneAnimNameVec.at(EA_BONE_IDLE_ADD_0));
		}
		m_fIdleAddTime += dDeltaTime;
	}

	m_fIdleTime += dDeltaTime;
	// 重置“Idle附加”动画时间
	if (m_fIdleAddDuration < m_fIdleAddTime)
		m_fIdleAddTime = 0.0;
}

void CGMCharacter::_UpdateDance(const double dDeltaTime)
{
	for (auto& itr : m_animDanceVec)
	{
		float fFadeSpeed = 1.0f;
		if (m_bStartDance) // 如果是第一次跳舞，则需要慢慢淡入
			fFadeSpeed = 0.2f;
		else if(!m_bMusicOn || m_bReStartDance) // 停下音乐或重新调整音乐时间时，需要稍微快一点淡出
			fFadeSpeed = 1.5f;
		else
			fFadeSpeed = 1.0f;

		if (itr.SetWeightCloserToTarget(dDeltaTime, fFadeSpeed))
		{
			GM_ANIMATION.SetAnimationWeight(m_strName, itr.fWeightNow, m_strBoneAnimNameVec.at(itr.eAnimation));
		}
		// 如果达到合适条件，则停止动画
		if (0 == itr.fWeightNow && 0 != itr.fWeightSource)
		{
			_StopAnimation(itr);
			m_bReStartDance = false;
			// 这里需要让角色不看向鼠标
			// to do
		}
	}
}

void CGMCharacter::_UpdateLookAnimation(const double dDeltaTime)
{
	// 如果强迫角色注视目标点，则注视一段时间
	if (m_bLookAtTarget)
	{
		_UpdateLookAt(dDeltaTime);
	}
	else
	{
		_UpdateLookAround(dDeltaTime);
	}
}

void CGMCharacter::_UpdateArmAnimation(const double dDeltaTime)
{
	if (!m_animArmR.bAnimOn && m_fArmTimeL <= m_fArmDurationL)
	{
		m_fArmTimeL += dDeltaTime;

		float fMix = _Smoothstep(0.0f, ARM_FADE_TIME, m_fArmTimeL) // 淡入
			* _Smoothstep(0.0f, ARM_FADE_TIME, m_fArmDurationL - m_fArmTimeL); // 淡出

		m_animArmL.fWeightNow = m_animArmL.fWeightSource * (1 - fMix) + m_animArmL.fWeightTarget * fMix;
		GM_ANIMATION.SetAnimationWeight(m_strName, m_animArmL.fWeightNow, m_strBoneAnimNameVec.at(EA_BONE_ARM_L_UP));
	}
	else
	{
		m_animArmL.bAnimOn = false;
	}

	if (!m_animArmL.bAnimOn && m_fArmTimeR <= m_fArmDurationR)
	{
		m_fArmTimeR += dDeltaTime;

		float fMix = _Smoothstep(0.0f, ARM_FADE_TIME, m_fArmTimeR) // 淡入
			* _Smoothstep(0.0f, ARM_FADE_TIME, m_fArmDurationR - m_fArmTimeR); // 淡出

		m_animArmR.fWeightNow = m_animArmR.fWeightSource * (1 - fMix) + m_animArmR.fWeightTarget * fMix;
		GM_ANIMATION.SetAnimationWeight(m_strName, m_animArmR.fWeightNow, m_strBoneAnimNameVec.at(EA_BONE_ARM_R_UP));
	}
	else
	{
		m_animArmR.bAnimOn = false;
	}
}

void CGMCharacter::_UpdateLookAt(const double dDeltaTime)
{
	if (_SetHeadWeightToTarget(dDeltaTime, 2 + m_fInterest*3))
	{
		_UpdateHeadAnimation();
	}
}

void CGMCharacter::_UpdateLookAround(const double dDeltaTime)
{
	if (m_fTurnMixTime >= m_fTurnDuration) return;
	float fMix = _Smoothstep(0.0, m_fTurnDuration, m_fTurnMixTime);
	if (_SetHeadWeightMix(fMix))
	{
		_UpdateHeadAnimation();
	}
	m_fTurnMixTime += dDeltaTime;
	if (m_fTurnMixTime > 0.7 * m_fTurnDuration)
	{
		// 这个函数目前会多次进入，但不影响性能，所以暂且不管
		// 眼球回到正前方
		_SetEyeFinalDir(0, 0);
	}
	if (m_fTurnMixTime > m_fTurnDuration)
	{
		m_fTurnMixTime = m_fTurnDuration;
	}
}

void CGMCharacter::_UpdateEye(const double dDeltaTime)
{
	int i = 0;
	for (auto& itr : m_pEyeTransVector)
	{
		osg::Quat qRotate = osg::Quat(
			m_fEyeBallPitch, osg::Vec3(1, 0, 0),// 俯仰，单位：弧度
			osg::PI_2 + m_fEyeBallHeading, osg::Vec3(0, 1, 0),// 偏转，单位：弧度
			0, osg::Vec3(0, 0, 1)); // 眼球不可能滚转，所以必须为0
		m_mEyeTransVector.at(i).setRotate(qRotate);
		itr->asMatrixTransform()->setMatrix(m_mEyeTransVector.at(i));
		i++;
	}
	// 眼球下一帧旋转的角度，单位：弧度
	float fDeltaAngle = 2 * dDeltaTime;
	// 更新偏转角
	if (m_fEyeBallFinalHeading > m_fEyeBallHeading)
	{
		m_fEyeBallHeading += fDeltaAngle;
		if (m_fEyeBallFinalHeading < m_fEyeBallHeading)
			m_fEyeBallHeading = m_fEyeBallFinalHeading;
	}
	else
	{
		m_fEyeBallHeading -= fDeltaAngle;
		if (m_fEyeBallFinalHeading > m_fEyeBallHeading)
			m_fEyeBallHeading = m_fEyeBallFinalHeading;
	}
	// 更新俯仰角
	if (m_fEyeBallFinalPitch > m_fEyeBallPitch)
	{
		m_fEyeBallPitch += fDeltaAngle;
		if (m_fEyeBallFinalPitch < m_fEyeBallPitch)
			m_fEyeBallPitch = m_fEyeBallFinalPitch;
	}
	else
	{
		m_fEyeBallPitch -= fDeltaAngle;
		if (m_fEyeBallFinalPitch > m_fEyeBallPitch)
			m_fEyeBallPitch = m_fEyeBallFinalPitch;
	}
}

bool CGMCharacter::_SetHeadWeightToTarget(const float fDeltaTime, const float fSpeed)
{
	bool bLeft = m_animHeadL.SetWeightCloserToTarget(fDeltaTime, fSpeed);
	bool bRight = m_animHeadR.SetWeightCloserToTarget(fDeltaTime, fSpeed);

	bool bUp = m_animHeadU.SetWeightCloserToTarget(fDeltaTime, fSpeed);
	bool bDown = m_animHeadD.SetWeightCloserToTarget(fDeltaTime, fSpeed);

	return bLeft || bRight || bUp || bDown;
}

bool CGMCharacter::_SetHeadWeightMix(const float fMix)
{
	bool bLeft = m_animHeadL.SetWeightMix(fMix);
	bool bRight = m_animHeadR.SetWeightMix(fMix);

	bool bUp = m_animHeadU.SetWeightMix(fMix);
	bool bDown = m_animHeadD.SetWeightMix(fMix);

	return bLeft || bRight || bUp || bDown;
}

void CGMCharacter::_UpdateHeadAnimation()
{
	GM_ANIMATION.SetAnimationWeight(m_strName, m_animHeadL.fWeightNow, m_strBoneAnimNameVec.at(EA_BONE_HEAD_L));
	GM_ANIMATION.SetAnimationWeight(m_strName, m_animHeadR.fWeightNow, m_strBoneAnimNameVec.at(EA_BONE_HEAD_R));

	GM_ANIMATION.SetAnimationWeight(m_strName, m_animHeadU.fWeightNow, m_strBoneAnimNameVec.at(EA_BONE_HEAD_U));
	GM_ANIMATION.SetAnimationWeight(m_strName, m_animHeadD.fWeightNow, m_strBoneAnimNameVec.at(EA_BONE_HEAD_D));

	// 如果达到合适条件，则停止动画
	if (0 == m_animHeadR.fWeightNow && EA_BONE_HEAD_L == m_eNextHeadingAnim)
	{
		_StopAnimation(m_animHeadR);
	}
	if (0 == m_animHeadL.fWeightNow && EA_BONE_HEAD_R == m_eNextHeadingAnim)
	{
		_StopAnimation(m_animHeadL);
	}
	if (0 == m_animHeadD.fWeightNow && EA_BONE_HEAD_U == m_eNextPitchAnim)
	{
		_StopAnimation(m_animHeadD);
	}
	if (0 == m_animHeadU.fWeightNow && EA_BONE_HEAD_D == m_eNextPitchAnim)
	{
		_StopAnimation(m_animHeadU);
	}
}

void CGMCharacter::_StopAnimation(SGMAnimData& sAnim)
{
	sAnim.fWeightSource = sAnim.fWeightTarget;
	sAnim.bAnimOn = false;
	GM_ANIMATION.SetAnimationStop(m_strName, m_strBoneAnimNameVec.at(sAnim.eAnimation));
}
