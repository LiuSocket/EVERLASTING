//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		CGMSoftRigTransform.h
/// @brief		GMEngine - Soft RigTransform
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.13
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <osg/Object>
#include <osgAnimation/RigTransform>

namespace GM
{
    class CGMSoftRigGeometry;

    class CGMSoftRigTransform : public osgAnimation::RigTransform
    {
    public:
        CGMSoftRigTransform() : osgAnimation::RigTransform(){}
        CGMSoftRigTransform(const RigTransform& org, const osg::CopyOp& copyop):
            osgAnimation::RigTransform(org, copyop) {}

        virtual void operator()(CGMSoftRigGeometry&) {}

        /// to call manually when a skeleton is reacheable from the rig
        /// in order to prepare technic data before rendering
        virtual bool prepareData(CGMSoftRigGeometry&) { return true; }

    protected:
        virtual ~CGMSoftRigTransform() {}

    };

} // namespace GM
