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
#include <random>

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

        /** @brief 设置idle动画权重 */
        void SetIdleAnimation(const float fWeight);
        /** @brief 设置眨眼动画权重 */
        void SetBlinkAnimation(const float fWeight);

    private:
        void _InnerUpdate(const double dDeltaTime);

    // 变量
    private:
        SGMKernelData* m_pKernelData = nullptr;                 //!< 内核数据
        SGMConfigData* m_pConfigData = nullptr;                 //!< 配置数据

        float m_fBlinkAnimationWeight = 0.0f;                   //!< 眨眼动画权重

        std::default_random_engine m_iRandom;                   //!< 随机值
        std::uniform_int_distribution<> m_iPseudoNoise;         //!< 伪随机数分布
    };
}	// GM
