//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		CSoftBoneMapVisitor.h
/// @brief		GMEngine - Soft BoneMap Visitor
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.12
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMSoftBone.h"
#include <osgAnimation/BoneMapVisitor>

namespace GM
{
    class CSoftBoneMapVisitor : public osgAnimation::BoneMapVisitor
    {
    public:
        CSoftBoneMapVisitor();

        void apply(osg::Node&);
        void apply(osg::Transform& node);
        const SoftBoneMap& getBoneMap() const;

    protected:
        SoftBoneMap _softMap;
    };
} // namespace GM
