//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		UpdateSoftBone.h
/// @brief		GMEngine - Update Soft Bone
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.09
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <osgAnimation/UpdateBone>

namespace GM
{
    /*************************************************************************
     Class
    *************************************************************************/

    /*!
     *  @class UpdateSoftBone
     *  @brief 更新软骨，使之能像弹簧一样弯曲
     */
    class UpdateSoftBone : public osgAnimation::UpdateBone
    {
    public:
        UpdateSoftBone(const std::string& name = "");
        UpdateSoftBone(const UpdateSoftBone&,const osg::CopyOp&);
        void operator()(osg::Node* node, osg::NodeVisitor* nv);
    };

} // namespace GM
