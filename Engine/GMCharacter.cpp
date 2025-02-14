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
#include "Animation/GMAnimation.h"

using namespace GM;

/*************************************************************************
Global Constants
*************************************************************************/

/*************************************************************************
CGMCharacter Methods
*************************************************************************/
/** @brief 构造 */
CGMCharacter::CGMCharacter()
{
    m_iRandom.seed(0);
    m_iPseudoNoise = std::uniform_int_distribution<>(0, 100);
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

    return true;
}

bool CGMCharacter::UpdatePost(double dDeltaTime)
{
    return true;
}

void CGMCharacter::SetIdleAnimation(const float fWeight)
{
    GM_ANIMATION.SetAnimationMode("MIGI", EGM_PLAY_LOOP, "bone_idle");
    GM_ANIMATION.SetAnimationPriority("MIGI", 200, "bone_idle");
    GM_ANIMATION.SetAnimationPlay("MIGI", fWeight, "bone_idle");
}

void CGMCharacter::SetBlinkAnimation(const float fWeight)
{
    m_fBlinkAnimationWeight = fWeight;
    GM_ANIMATION.SetAnimationMode("MIGI", EGM_PLAY_ONCE, "eye_blink");
    GM_ANIMATION.SetAnimationPriority("MIGI", 100, "eye_blink");
}

void CGMCharacter::_InnerUpdate(const double dDeltaTime)
{
    static double s_fBlinkTime = 0.0;
    static double s_fNextTime = 2.0;
    if (s_fBlinkTime > s_fNextTime)
    {
        GM_ANIMATION.SetAnimationPlay("MIGI", m_fBlinkAnimationWeight, "eye_blink");
        s_fBlinkTime = 0.0;
        s_fNextTime = m_iPseudoNoise(m_iRandom) * 0.1 + 0.5;
    }
    s_fBlinkTime += dDeltaTime;

}