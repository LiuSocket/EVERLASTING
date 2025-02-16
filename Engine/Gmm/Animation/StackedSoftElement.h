//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		StackedSoftElement.h
/// @brief		GMEngine - Stacked Soft Element
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.15
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <random>
#include <osgAnimation/StackedTranslateElement>

namespace GM
{
    class StackedSoftElement : public osgAnimation::StackedTranslateElement
    {
    public:
        StackedSoftElement();
        StackedSoftElement(const StackedSoftElement&, const osg::CopyOp&);
        StackedSoftElement(const std::string& name, const osg::Vec3& translate = osg::Vec3(0,0,0));
        StackedSoftElement(const osg::Vec3& translate);
        StackedSoftElement(const std::string& name, const osg::Vec3& translate, const osg::Vec3& vLimit);

        void Init();
        void update(float t = 0.0);

        // 设置弹性限制
        inline void SetSoftLimit(const osg::Vec3& vLimit) { _vSoftLimit = vLimit; }

    protected:
        bool    _bInit = false;
        std::default_random_engine  m_iRandom;						//!< 随机值
        osg::Vec3 _vSoftLimit = osg::Vec3(-0.01f, 0.01f, 0.01f);
        osg::Vec3 _vSoftPhase = osg::Vec3(0.0f, 0.0f, 0.0f);

        osg::Vec3 _vSoftTrans = osg::Vec3(0.0f, 0.0f, 0.0f);
    };
} // namespace GM
