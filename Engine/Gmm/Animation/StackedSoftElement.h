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

#include <osgAnimation/StackedTranslateElement>

namespace GM
{
    class StackedSoftElement : public osgAnimation::StackedTranslateElement
    {
    public:
        StackedSoftElement();
        StackedSoftElement(const StackedSoftElement&, const osg::CopyOp&);
        StackedSoftElement(const std::string&, const osg::Vec3& translate = osg::Vec3(0,0,0));
        StackedSoftElement(const osg::Vec3& translate);

        void update(float t = 0.0);

    protected:
        osg::Vec3 _vSoftTranslate = osg::Vec3(-0.1f, 0.0f, 0.0f);
    };
} // namespace GM
