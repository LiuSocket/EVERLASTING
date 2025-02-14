//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		SoftVertexInfluenceMap.h
/// @brief		GMEngine - Soft VertexInfluenceMap
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.13
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <osgAnimation/VertexInfluence>

namespace GM
{
    class CGMSoftSkeleton;

    class SoftVertexInfluenceMap : public osgAnimation::VertexInfluenceMap
    {
    public:
        SoftVertexInfluenceMap() : osgAnimation::VertexInfluenceMap() {}
        SoftVertexInfluenceMap(const osgAnimation::VertexInfluenceMap& org, const osg::CopyOp& copyop):
            osgAnimation::VertexInfluenceMap(org, copyop) {}

        //Experimental removal of unexpressed bone from the soft skeleton
        void removeUnexpressedBones(CGMSoftSkeleton& skel) const;
    };
} // namespace GM
