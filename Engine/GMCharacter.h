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
     *  @class CGMCharacter��ϵͳ��λ������
     *  @brief GM Character Module
     */
    class CGMCharacter
    {
    // ����
    public:
        /** @brief ���� */
        CGMCharacter();
        /** @brief ���� */
        ~CGMCharacter();

        /** @brief ��ʼ�� */
        bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData);
        /** @brief ���� */
        bool Update(double dDeltaTime);
        /** @brief ����(�������������̬֮��) */
        bool UpdatePost(double dDeltaTime);

        /** @brief ����idle����Ȩ�� */
        void SetIdleAnimation(const float fWeight);
        /** @brief ����գ�۶���Ȩ�� */
        void SetBlinkAnimation(const float fWeight);

    private:
        void _InnerUpdate(const double dDeltaTime);

    // ����
    private:
        SGMKernelData* m_pKernelData = nullptr;                 //!< �ں�����
        SGMConfigData* m_pConfigData = nullptr;                 //!< ��������

        float m_fBlinkAnimationWeight = 0.0f;                   //!< գ�۶���Ȩ��

        std::default_random_engine m_iRandom;                   //!< ���ֵ
        std::uniform_int_distribution<> m_iPseudoNoise;         //!< α������ֲ�
    };
}	// GM
