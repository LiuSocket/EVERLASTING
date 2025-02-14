//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMSoftSkeleton.h
/// @brief		GMEngine - Soft Skeleton
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.10
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <osgAnimation/Skeleton>

namespace GM
{
    /*************************************************************************
     Class
    *************************************************************************/

    /*!
     *  @class CGMSoftSkeleton
     *  @brief Èí¹Ç¼Ü£¬ÄÜÏñµ¯»ÉÒ»ÑùÍäÇúµÄ¹Ç¼Ü
     */
    class CGMSoftSkeleton : public osgAnimation::Skeleton
    {
    public:
        class UpdateSoftSkeleton : public osgAnimation::Skeleton::UpdateSkeleton
        {
        public:
            UpdateSoftSkeleton();
            UpdateSoftSkeleton(const UpdateSoftSkeleton& s, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
            virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
            bool needToValidate() const;
        protected:
            bool _needValidate;
        };

        CGMSoftSkeleton();
        CGMSoftSkeleton(const CGMSoftSkeleton&, const osg::CopyOp&);
        void setDefaultUpdateCallback();
    };
} // namespace GM
