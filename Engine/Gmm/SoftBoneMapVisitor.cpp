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

#include "SoftBoneMapVisitor.h"
#include "GMSoftSkeleton.h"
#include "GMSoftBone.h"

using namespace GM;

CSoftBoneMapVisitor::CSoftBoneMapVisitor() : osgAnimation::BoneMapVisitor() {}

void CSoftBoneMapVisitor::apply(osg::Node&) { return; }
void CSoftBoneMapVisitor::apply(osg::Transform& node)
{
    CGMSoftBone* bone = dynamic_cast<CGMSoftBone*>(&node);
    if (bone)
    {
        _softMap[bone->getName()] = bone;
        traverse(node);
    }
    CGMSoftSkeleton* skeleton = dynamic_cast<CGMSoftSkeleton*>(&node);
    if (skeleton)
        traverse(node);
}

const SoftBoneMap& CSoftBoneMapVisitor::getBoneMap() const
{
    return _softMap;
}
