//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMSoftSkeleton.cpp
/// @brief		GMEngine - Soft Skeleton
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.10
//////////////////////////////////////////////////////////////////////////

#include "GMSoftSkeleton.h"
#include "GMSoftBone.h"

using namespace GM;

CGMSoftSkeleton::CGMSoftSkeleton() {}

CGMSoftSkeleton::CGMSoftSkeleton(const CGMSoftSkeleton& b, const osg::CopyOp& copyop) : osgAnimation::Skeleton(b,copyop) {}

CGMSoftSkeleton::UpdateSoftSkeleton::UpdateSoftSkeleton() : _needValidate(true) {}

CGMSoftSkeleton::UpdateSoftSkeleton::UpdateSoftSkeleton(const UpdateSoftSkeleton& us, const osg::CopyOp& copyop)
    : osgAnimation::Skeleton::UpdateSkeleton(us, copyop)
{
    _needValidate = true;
}

bool CGMSoftSkeleton::UpdateSoftSkeleton::needToValidate() const
{
    return _needValidate;
}

class ValidateSoftSkeletonVisitor : public osg::NodeVisitor
{
public:
    ValidateSoftSkeletonVisitor(): osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
    void apply(osg::Node& node) { return; }
    void apply(osg::Transform& node)
    {
        // the idea is to traverse the skeleton or bone but to stop if other node is found
        CGMSoftBone* pSoftBone = dynamic_cast<CGMSoftBone*>(&node);
        if (!pSoftBone)
            return;

        bool foundNonBone = false;

        for (unsigned int i = 0; i < pSoftBone->getNumChildren(); ++i)
        {
            if (dynamic_cast<CGMSoftBone*>(pSoftBone->getChild(i)))
            {
                if (foundNonBone)
                {
                    OSG_WARN <<
                        "Warning: a SoftBone was found after a non-Bone child "
                        "within a SoftSkeleton. Children of a SoftBone must be ordered "
                        "with all child SoftBones first for correct update order." << std::endl;
                    setTraversalMode(TRAVERSE_NONE);
                    return;
                }
            }
            else
            {
                foundNonBone = true;
            }
        }
        traverse(node);
    }
};

void CGMSoftSkeleton::UpdateSoftSkeleton::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
    if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
    {
        CGMSoftSkeleton* pSoftSkeleton = dynamic_cast<CGMSoftSkeleton*>(node);
        if (_needValidate && pSoftSkeleton)
        {
            ValidateSoftSkeletonVisitor visitor;
            for (unsigned int i = 0; i < pSoftSkeleton->getNumChildren(); ++i)
            {
                osg::Node* child = pSoftSkeleton->getChild(i);
                child->accept(visitor);
            }
            _needValidate = false;
        }
    }
    traverse(node,nv);
}

void CGMSoftSkeleton::setDefaultUpdateCallback()
{
    setUpdateCallback(new CGMSoftSkeleton::UpdateSoftSkeleton );
}
