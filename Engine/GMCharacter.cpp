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

#define  BONE_PRIORITY_HIGHEST				(12)		// 骨骼动画“最高”优先级
#define  BONE_PRIORITY_HIGH					(11)		// 骨骼动画“高”优先级
#define  BONE_PRIORITY_NORMAL				(10)		// 骨骼动画“普通”优先级

#define  MORPH_PRIORITY_HIGHEST				(2)			// 变形动画“最高”优先级
#define  MORPH_PRIORITY_HIGH				(1)			// 变形动画“高”优先级
#define  MORPH_PRIORITY_NORMAL				(0)			// 变形动画“普通”优先级

#define  SEEK_TARGET_TIME					(2.0f)		// 搜索目标最长时间，单位：秒

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
	m_strBoneAnimNameVec.push_back("bone_head_L");
	m_strBoneAnimNameVec.push_back("bone_head_R");
	m_strBoneAnimNameVec.push_back("bone_head_U");
	m_strBoneAnimNameVec.push_back("bone_head_D");

	m_strMorphAnimNameVec.push_back("eye_blink");
	m_strMorphAnimNameVec.push_back("eye_half");
	m_strMorphAnimNameVec.push_back("eye_surprise");
	m_strMorphAnimNameVec.push_back("mouth_idle");
	m_strMorphAnimNameVec.push_back("mouth_aa");
	m_strMorphAnimNameVec.push_back("mouth_oo");
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

	return true;
}

bool CGMCharacter::Update(double dDeltaTime)
{
	static double fConstantStep = 0.1;
	static double fDeltaStep = 0.0;
	if (fDeltaStep > fConstantStep)
	{
		_InnerUpdate(fDeltaStep);
		fDeltaStep = 0.0;
	}
	fDeltaStep += dDeltaTime;

	// 过渡状态时需要每帧更新转头动画的权重
	_UpdateLookAnimation(dDeltaTime);

	// 每帧累加搜索时间
	m_fSeekTargetTime += dDeltaTime;

	// 每帧都在减少好奇心（注意力很难长时间集中）
	m_fInterest = fmax(0.0f, m_fInterest - 0.2f * dDeltaTime);
	// 每帧都在减少愤怒值（消气）
	m_fAngry = fmax(0.0f, m_fAngry - 0.02f * dDeltaTime);

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
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_NORMAL, m_strBoneAnimNameVec.at(EA_BONE_IDLE));
	GM_ANIMATION.SetAnimationPlay(strName, m_strBoneAnimNameVec.at(EA_BONE_IDLE));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_LOOP, m_strBoneAnimNameVec.at(EA_BONE_HEAD_L));
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_HIGH, m_strBoneAnimNameVec.at(EA_BONE_HEAD_L));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_LOOP, m_strBoneAnimNameVec.at(EA_BONE_HEAD_R));
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_HIGH, m_strBoneAnimNameVec.at(EA_BONE_HEAD_R));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_LOOP, m_strBoneAnimNameVec.at(EA_BONE_HEAD_U));
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_HIGH, m_strBoneAnimNameVec.at(EA_BONE_HEAD_U));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_LOOP, m_strBoneAnimNameVec.at(EA_BONE_HEAD_D));
	GM_ANIMATION.SetAnimationPriority(strName, BONE_PRIORITY_HIGH, m_strBoneAnimNameVec.at(EA_BONE_HEAD_D));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strMorphAnimNameVec.at(EA_MORPH_BLINK));
	GM_ANIMATION.SetAnimationPriority(strName, MORPH_PRIORITY_NORMAL, m_strMorphAnimNameVec.at(EA_MORPH_BLINK));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strMorphAnimNameVec.at(EA_MORPH_HALF));
	GM_ANIMATION.SetAnimationPriority(strName, MORPH_PRIORITY_HIGH, m_strMorphAnimNameVec.at(EA_MORPH_HALF));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strMorphAnimNameVec.at(EA_MORPH_SURPRISE));
	GM_ANIMATION.SetAnimationPriority(strName, MORPH_PRIORITY_HIGH, m_strMorphAnimNameVec.at(EA_MORPH_SURPRISE));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strMorphAnimNameVec.at(EA_MORPH_IDLE));
	GM_ANIMATION.SetAnimationPriority(strName, MORPH_PRIORITY_NORMAL, m_strMorphAnimNameVec.at(EA_MORPH_IDLE));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strMorphAnimNameVec.at(EA_MORPH_AA));
	GM_ANIMATION.SetAnimationPriority(strName, MORPH_PRIORITY_HIGH, m_strMorphAnimNameVec.at(EA_MORPH_AA));

	GM_ANIMATION.SetAnimationMode(strName, EGM_PLAY_ONCE, m_strMorphAnimNameVec.at(EA_MORPH_OO));
	GM_ANIMATION.SetAnimationPriority(strName, MORPH_PRIORITY_HIGH, m_strMorphAnimNameVec.at(EA_MORPH_OO));

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

void CGMCharacter::_InnerUpdate(const double dDeltaTime)
{
	// 眼睛转向
	_ChangeLookDir(dDeltaTime);
	// 眨眼
	_InnerUpdateBlink(dDeltaTime);
	// 嘴唇
	_InnerUpdateLip(dDeltaTime);
}

void CGMCharacter::_InnerUpdateBlink(const double dDeltaTime)
{
	static double s_fBlinkTime = 0.0;
	static double s_fDeltaBlinkTime = 2.0;
	// “惊讶”和“半闭眼”时不眨眼
	if (s_fBlinkTime > s_fDeltaBlinkTime && !m_bSurprise && m_fInterest > 0.0)
	{
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
	if (s_fAATime > s_fDeltaAATime && !m_bSurprise && m_fAngry < 0.9)
	{
		float fMorphDuration = m_iPseudoNoise(m_iRandom) * 0.05 + 2;
		GM_ANIMATION.SetAnimationDuration(m_strName, fMorphDuration, m_strMorphAnimNameVec.at(EA_MORPH_IDLE));
		GM_ANIMATION.SetAnimationPlay(m_strName, m_strMorphAnimNameVec.at(EA_MORPH_IDLE));

		s_fAATime = 0.0;
		s_fDeltaAATime = m_iPseudoNoise(m_iRandom) * 0.1 + fMorphDuration;
	}
	s_fAATime += dDeltaTime;
}

void CGMCharacter::_ChangeLookDir(const double dDeltaTime)
{
	static float fAfterSurpriseTime = 0.0f;

	// 刚鄙视完，气还没消，直接无视目标
	bool bIgnoreTarget = m_bDisdain && m_fAngry > 0.1;
	// 如果强迫角色注视目标点，则注视一段时间
	if (m_bTargetVisible && !bIgnoreTarget)
	{
		// “惊讶”过30秒之后才能再次“惊讶”
		if (!m_bSurprise && (fAfterSurpriseTime > 30.0f) && m_fInterest>0.9 && m_fAngry<0.2)
		{
			GM_ANIMATION.SetAnimationPlay(m_strName, m_strMorphAnimNameVec.at(EA_MORPH_SURPRISE));

			m_bSurprise = true;
			fAfterSurpriseTime = 0.0f;
		}

		_ChangeLookAtTarget(dDeltaTime);
	}
	else if(m_fSeekTargetTime > SEEK_TARGET_TIME)// 否则则执行“四处张望”的功能
	{
		_ChangeLookAround(dDeltaTime);
	}
	else {}

	fAfterSurpriseTime += dDeltaTime;
	// 惊讶不过1秒
	if (fAfterSurpriseTime > 1.0f) m_bSurprise = false;

	//std::cout << m_fAngry << std::endl;
}

void CGMCharacter::_ChangeLookAtTarget(const double dDeltaTime)
{
	// 计算目标点的加速度
	osg::Vec3d vTargetVelocity = (m_vTargetWorldPos - m_vTargetLastWorldPos) / dDeltaTime;
	osg::Vec3d vDeltaVelocity = vTargetVelocity - m_vTargetLastVelocity;
	float fDeltaVelocity = vDeltaVelocity.length();

	m_vTargetLastVelocity = vTargetVelocity;
	m_vTargetLastWorldPos = m_vTargetWorldPos;

	if (m_bDisdain)
	{
		if (m_fAngry < 0.4)
		{
			m_bDisdain = false;
		}
	}
	else
	{
		// 如果目标在变速运动，且愤怒值小于某个阈值，则角色的好奇心开始增加
		if (m_fAngry < 0.6)
		{
			m_fInterest = fmin(1.0f, m_fInterest + fDeltaVelocity * 1.5e-3f);
		}
		// 如果变化不剧烈，则不会增加愤怒，超过一定的速度后才会增加
		float fAddAngry = fmax(0.0f, fDeltaVelocity - 10.0f);
		// 如果短时间内加速度模的累计值过大，说明目标在剧烈运动，可以认为在耍自己，愤怒值飙升
		m_fAngry = fmin(m_fAngry + fAddAngry * fAddAngry * 5e-7f, 1.0f);
	}

	osg::Vec3d vDir = osg::Vec3d(0, -1, 0);
	// 愤怒值超过一个阈值后，角色会放弃注视目标并看向前方一段时间，这时会减少愤怒值
	// 愤怒值小于一个阈值后才会继续随意看四周，此时愤怒值会继续减小一直到0，然后才能继续注视
	if (m_fAngry > 0.5)
	{
		if (!m_bDisdain)
		{
			m_bDisdain = true;
			GM_ANIMATION.SetAnimationPlay(m_strName, m_strMorphAnimNameVec.at(EA_MORPH_HALF));
			GM_ANIMATION.SetAnimationDuration(m_strName, 4.0f, m_strMorphAnimNameVec.at(EA_MORPH_HALF));
		}
	}
	else if(m_fAngry < 0.45)
	{
		vDir = m_vTargetWorldPos - osg::Vec3d(0.0, -1.0, 12.0);
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
				m_iRandomAngle(m_iRandom) / 36 - 5, // [-5, 5]
				m_iRandomAngle(m_iRandom) / 24 - 7.5); // [-7.5, 7.5]
			s_i = 0;
		}
		s_i++;
	}

	if (s_fLookAroundTime >= (m_fLookDuration + m_fTurnDuration)) // 开始改变朝向
	{
		float fDeltaHeading = m_iRandomAngle(m_iRandom) / 3 - 60; // [-60, 60]
		float fDeltaPitch = m_iRandomAngle(m_iRandom) / 12 - 15; // [-15, 15]

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

		m_animL.fWeightSource = m_animL.fWeightTarget;
		m_animR.fWeightSource = m_animR.fWeightTarget;
		m_animU.fWeightSource = m_animU.fWeightTarget;
		m_animD.fWeightSource = m_animD.fWeightTarget;

		// 这里要保证传入的是最终的俯仰和偏航角
		_ChangeTargetAnimation(fHeading, fPitch);

		s_fLookAroundTime = 0.0;
		//重置混合时间，开始混合动画
		m_fMixTime = 0.0f;
		m_fLookDuration = m_iPseudoNoise(m_iRandom) * 0.05 + 0.5;
		m_fTurnDuration = (m_iPseudoNoise(m_iRandom) * 0.0001 + 0.01) * fDeltaAngle + 0.3;
	}
	s_fLookAroundTime += dDeltaTime;
}

void CGMCharacter::_ChangeTargetAnimation(const float fTargetHeading, const float fTargetPitch)
{
	m_fTargetHeading = fTargetHeading;
	m_fTargetPitch = fTargetPitch;
	if (m_fTargetHeading < 0) //需要朝左
	{
		m_animL.fWeightSource = m_animL.fWeightNow;
		m_animL.fWeightTarget = abs(m_fTargetHeading) / 90;
		m_animL.bAnimOn = true;

		m_animR.fWeightSource = m_animR.fWeightNow;
		m_animR.fWeightTarget = 0;

		m_eNextHeadingAnim = EA_BONE_HEAD_L;
	}
	else //需要朝右
	{
		m_animL.fWeightSource = m_animL.fWeightNow;
		m_animL.fWeightTarget = 0;

		m_animR.fWeightSource = m_animR.fWeightNow;
		m_animR.fWeightTarget = abs(m_fTargetHeading) / 90;
		m_animR.bAnimOn = true;

		m_eNextHeadingAnim = EA_BONE_HEAD_R;
	}

	if (m_fTargetPitch < 0) //需要朝下
	{
		m_animU.fWeightSource = m_animU.fWeightNow;
		m_animU.fWeightTarget = 0;

		m_animD.fWeightSource = m_animD.fWeightNow;
		m_animD.fWeightTarget = abs(m_fTargetPitch) / 90;
		m_animD.bAnimOn = true;

		m_eNextPitchAnim = EA_BONE_HEAD_D;
	}
	else //需要朝上
	{
		m_animU.fWeightSource = m_animU.fWeightNow;
		m_animU.fWeightTarget = abs(m_fTargetPitch) / 90;
		m_animU.bAnimOn = true;

		m_animD.fWeightSource = m_animD.fWeightNow;
		m_animD.fWeightTarget = 0;

		m_eNextPitchAnim = EA_BONE_HEAD_U;
	}

	if (!GM_ANIMATION.IsAnimationPlaying(m_strName, m_strBoneAnimNameVec.at(m_eNextHeadingAnim)))
		GM_ANIMATION.SetAnimationPlay(m_strName, m_strBoneAnimNameVec.at(m_eNextHeadingAnim), 1e-5f);
	if (!GM_ANIMATION.IsAnimationPlaying(m_strName, m_strBoneAnimNameVec.at(m_eNextPitchAnim)))
		GM_ANIMATION.SetAnimationPlay(m_strName, m_strBoneAnimNameVec.at(m_eNextPitchAnim), 1e-5f);
}

void CGMCharacter::_UpdateLookAnimation(const double dDeltaTime)
{
	if (m_bTargetVisible || m_fSeekTargetTime < SEEK_TARGET_TIME)
	{
		_UpdateLookAt(dDeltaTime);
	}
	else
	{
		_UpdateLookAround(dDeltaTime);
	}
}

void CGMCharacter::_UpdateLookAt(const double dDeltaTime)
{
	if (_SetWeightCloserToTarget(dDeltaTime, m_fInterest * 5))
	{
		_UpdateAnimationWeight();
		_StopAnimation();
	}
}

void CGMCharacter::_UpdateLookAround(const double dDeltaTime)
{
	if (m_fMixTime >= m_fTurnDuration) return;
	float fMix = _Smoothstep(0.0, m_fTurnDuration, m_fMixTime);
	if (_SetWeightMix(fMix))
	{
		_UpdateAnimationWeight();
		_StopAnimation();
	}
	m_fMixTime += dDeltaTime;
	if (m_fMixTime > 0.7 * m_fTurnDuration)
	{
		// 这个函数目前会多次进入，但不影响性能，所以暂且不管
		// 眼球回到正前方
		_SetEyeFinalDir(0, 0);
	}
	if (m_fMixTime > m_fTurnDuration)
	{
		m_fMixTime = m_fTurnDuration;
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

bool CGMCharacter::_SetWeightCloserToTarget(const float fDeltaTime, const float fSpeed)
{
	bool bLeft = m_animL.SetWeightCloserToTarget(fDeltaTime, fSpeed);
	bool bRight = m_animR.SetWeightCloserToTarget(fDeltaTime, fSpeed);

	bool bUp = m_animU.SetWeightCloserToTarget(fDeltaTime, fSpeed);
	bool bDown = m_animD.SetWeightCloserToTarget(fDeltaTime, fSpeed);

	return bLeft || bRight || bUp || bDown;
}

bool CGMCharacter::_SetWeightMix(const float fMix)
{
	bool bLeft = m_animL.SetWeightMix(fMix);
	bool bRight = m_animR.SetWeightMix(fMix);

	bool bUp = m_animU.SetWeightMix(fMix);
	bool bDown = m_animD.SetWeightMix(fMix);

	return bLeft || bRight || bUp || bDown;
}

void CGMCharacter::_UpdateAnimationWeight()
{
	GM_ANIMATION.SetAnimationWeight(m_strName, m_strBoneAnimNameVec.at(EA_BONE_HEAD_L), m_animL.fWeightNow);
	GM_ANIMATION.SetAnimationWeight(m_strName, m_strBoneAnimNameVec.at(EA_BONE_HEAD_R), m_animR.fWeightNow);

	GM_ANIMATION.SetAnimationWeight(m_strName, m_strBoneAnimNameVec.at(EA_BONE_HEAD_U), m_animU.fWeightNow);
	GM_ANIMATION.SetAnimationWeight(m_strName, m_strBoneAnimNameVec.at(EA_BONE_HEAD_D), m_animD.fWeightNow);
}

void CGMCharacter::_StopAnimation()
{
	if (0 == m_animR.fWeightNow && EA_BONE_HEAD_L == m_eNextHeadingAnim)
	{
		m_animR.fWeightSource = m_animR.fWeightTarget;
		m_animR.bAnimOn = false;
		GM_ANIMATION.SetAnimationPlay(m_strName, m_strBoneAnimNameVec.at(EA_BONE_HEAD_R), 0);
	}
	if (0 == m_animL.fWeightNow && EA_BONE_HEAD_R == m_eNextHeadingAnim)
	{
		m_animL.fWeightSource = m_animL.fWeightTarget;
		m_animL.bAnimOn = false;
		GM_ANIMATION.SetAnimationPlay(m_strName, m_strBoneAnimNameVec.at(EA_BONE_HEAD_L), 0);
	}
	if (0 == m_animD.fWeightNow && EA_BONE_HEAD_U == m_eNextPitchAnim)
	{
		m_animD.fWeightSource = m_animD.fWeightTarget;
		m_animD.bAnimOn = false;
		GM_ANIMATION.SetAnimationPlay(m_strName, m_strBoneAnimNameVec.at(EA_BONE_HEAD_D), 0);
	}
	if (0 == m_animU.fWeightNow && EA_BONE_HEAD_D == m_eNextPitchAnim)
	{
		m_animU.fWeightSource = m_animU.fWeightTarget;
		m_animU.bAnimOn = false;
		GM_ANIMATION.SetAnimationPlay(m_strName, m_strBoneAnimNameVec.at(EA_BONE_HEAD_U), 0);
	}
}