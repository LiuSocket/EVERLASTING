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
     *  @brief ��ǣ��񵯻�һ������
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
        * @brief ������ת��������������
        * @param matrix: �����ռ����ת����
        */
        void setSoftMatrixInBoneSpace(const osg::Matrix& matrix) { _softInBoneSpace = matrix; }

    protected:

        // �����ת�����ڹ����ռ䣩
        osg::Matrix _softInBoneSpace = osg::Matrix::identity();
    };

    typedef std::map<std::string, osg::ref_ptr<CGMSoftBone> > SoftBoneMap;
} // namespace GM
