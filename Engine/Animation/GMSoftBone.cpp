//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMSoftBone.cpp
/// @brief		GMEngine - Soft Bone Module
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.09
//////////////////////////////////////////////////////////////////////////
#include "GMSoftBone.h"
#include "UpdateSoftBone.h"

using namespace GM;

CGMSoftBone::CGMSoftBone(const CGMSoftBone& b, const osg::CopyOp& copyop) : Bone(b,copyop)
{
}

CGMSoftBone::CGMSoftBone(const std::string& name) : Bone(name)
{
}


void CGMSoftBone::setDefaultUpdateCallback(const std::string& name)
{
    std::string cbName = name;
    if (cbName.empty())
        cbName = getName();
    setUpdateCallback(new UpdateSoftBone(cbName));
}