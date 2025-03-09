//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		UpdateSoftBone.cpp
/// @brief		GMEngine - Update Soft Bone
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.09
//////////////////////////////////////////////////////////////////////////

#include "UpdateSoftBone.h"
#include <osgAnimation/Bone>
#include <osg/NodeVisitor>

using namespace GM;
using namespace osgAnimation;

UpdateSoftBone::UpdateSoftBone(const std::string& name) : UpdateBone(name)
{
}

UpdateSoftBone::UpdateSoftBone(const UpdateBone& apc,const osg::CopyOp& copyop) : UpdateBone(apc,copyop)
{
}

/** Callback method called by the NodeVisitor when visiting a node.*/
void UpdateSoftBone::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
    if (nv && nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
    {
        Bone* b = dynamic_cast<Bone*>(node);
        if (!b)
        {
            OSG_WARN << "Warning: UpdateSoftBone set on non-Bone object." << std::endl;
            return;
        }

        // if I use real time,the shake will be not smooth,so I use the frame number instead
        //float time = nv->getFrameStamp()->getReferenceTime();
        unsigned int iFrame = nv->getFrameStamp()->getFrameNumber();
        // here we would prefer to have a flag inside transform stack in order to avoid update and a dirty state in matrixTransform if it's not require.
        _transforms.update(iFrame);
        const osg::Matrix& matrix = _transforms.getMatrix();
        b->setMatrix(matrix);

        Bone* parent = dynamic_cast<Bone*>(b->getBoneParent());
        if (parent)
            b->setMatrixInSkeletonSpace(matrix * parent->getMatrixInSkeletonSpace());
        else
            b->setMatrixInSkeletonSpace(matrix);
    }
    traverse(node,nv);
}
