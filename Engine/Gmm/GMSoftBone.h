//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMSoftBone.h
/// @brief		GMEngine - Soft Bone Module
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.09
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <osgAnimation/Bone>

namespace GM
{
    /*************************************************************************
     Class
    *************************************************************************/

    /*!
     *  @class CGMSoftBone
     *  @brief 软骨，像弹簧一样弯曲
     */
    class CGMSoftBone : public osgAnimation::Bone
    {
    public:
        CGMSoftBone(const CGMSoftBone& b, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
        CGMSoftBone(const std::string& name = "");

        void setDefaultUpdateCallback(const std::string& name = "");

        CGMSoftBone* getBoneParent();
        const CGMSoftBone* getBoneParent() const;

        /**
        * @brief 设置旋转矩阵，用于软化骨骼
        * @param matrix: 骨骼空间的旋转矩阵
        */
        void setSoftMatrixInBoneSpace(const osg::Matrix& matrix) { _softInBoneSpace = matrix; }

    protected:

        // 软骨旋转矩阵（在骨骼空间）
        osg::Matrix _softInBoneSpace = osg::Matrix::identity();
    };

    typedef std::map<std::string, osg::ref_ptr<CGMSoftBone> > SoftBoneMap;
} // namespace GM
