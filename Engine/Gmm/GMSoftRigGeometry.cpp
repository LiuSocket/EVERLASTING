//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMSoftRigGeometry.cpp
/// @brief		GMEngine - Soft RigGeometry
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.10
//////////////////////////////////////////////////////////////////////////

#include "GMSoftRigGeometry.h"
#include "GMSoftRigTransformSoftware.h"
#include <sstream>

using namespace GM;

CGMSoftRigGeometry::CGMSoftRigGeometry() : osgAnimation::RigGeometry()
{
    setUpdateCallback(new UpdateSoftRigGeometry);
    _softRigTransformImplementation = new CGMSoftRigTransformSoftware;
}


CGMSoftRigGeometry::CGMSoftRigGeometry(const CGMSoftRigGeometry& b, const osg::CopyOp& copyop)
    : osgAnimation::RigGeometry(b,copyop),
    _softRigTransformImplementation(osg::clone(b._softRigTransformImplementation.get(), copyop))
{
}

void CGMSoftRigGeometry::computeMatrixFromRootSkeleton()
{
    if (!_pSoftRoot.valid())
    {
        OSG_WARN << "Warning " << className() <<"::computeMatrixFromRootSkeleton if you have this message it means you miss to call buildTransformer(CGMSoftSkeleton* root), or your CGMSoftRigGeometry (" << getName() <<") is not attached to a CGMSoftSkeleton subgraph" << std::endl;
        return;
    }
    osg::MatrixList mtxList = getParent(0)->getWorldMatrices(_pSoftRoot.get());
    osg::Matrix notRoot = _pSoftRoot->getMatrix();
    _matrixFromSkeletonToGeometry = mtxList[0] * osg::Matrix::inverse(notRoot);
    _invMatrixFromSkeletonToGeometry = osg::Matrix::inverse(_matrixFromSkeletonToGeometry);
    _needToComputeMatrix = false;
}

void CGMSoftRigGeometry::update()
{
    CGMSoftRigTransform& implementation = *_softRigTransformImplementation;
    (implementation)(*this);
}