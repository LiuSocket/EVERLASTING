//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		CGMSoftRigGeometry.h
/// @brief		GMEngine - Soft RigGeometry
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.10
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMSoftSkeleton.h"
#include "GMSoftRigTransform.h"
#include "SoftVertexInfluence.h"
#include <osgAnimation/RigGeometry>

namespace GM
{
    /*************************************************************************
     Class
    *************************************************************************/

    /*!
     *  @class CGMSoftRigGeometry
     *  @brief 软骨架的几何体
     */
    class CGMSoftRigGeometry : public osgAnimation::RigGeometry
    {
    public:
        CGMSoftRigGeometry();
        CGMSoftRigGeometry(const CGMSoftRigGeometry& b, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

        inline void setInfluenceMap(SoftVertexInfluenceMap* vertexInfluenceMap) { _softVertexInfluenceMap = vertexInfluenceMap; }
        inline const SoftVertexInfluenceMap* getInfluenceMap() const { return _softVertexInfluenceMap.get(); }
        inline SoftVertexInfluenceMap* getInfluenceMap() { return _softVertexInfluenceMap.get(); }

        inline const CGMSoftSkeleton* getSkeleton() const { return _pSoftRoot.get(); }
        inline CGMSoftSkeleton* getSkeleton() { return _pSoftRoot.get(); }
        // will be used by the update callback to init correctly the rig mesh
        inline void setSkeleton(CGMSoftSkeleton* root) { _pSoftRoot = root; }

        void computeMatrixFromRootSkeleton();

        // set implementation of rig method
        inline CGMSoftRigTransform* getRigTransformImplementation() { return _softRigTransformImplementation.get(); }
        inline void setRigTransformImplementation(CGMSoftRigTransform* rig) { _softRigTransformImplementation = rig; }
        inline const CGMSoftRigTransform* getRigTransformImplementation() const { return _softRigTransformImplementation.get(); }
        
        void update();

        void buildVertexInfluenceSet() { _softRigTransformImplementation->prepareData(*this); }

        struct FindNearestParentSoftSkeleton : public osg::NodeVisitor
        {
            osg::ref_ptr<CGMSoftSkeleton> _pSoftRoot;
            FindNearestParentSoftSkeleton() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_PARENTS) {}
            void apply(osg::Transform& node)
            {
                if (_pSoftRoot.valid())
                    return;
                _pSoftRoot = dynamic_cast<CGMSoftSkeleton*>(&node);
                traverse(node);
            }
        };

    protected:

        osg::observer_ptr<CGMSoftSkeleton> _pSoftRoot;
        osg::ref_ptr<CGMSoftRigTransform> _softRigTransformImplementation;
        osg::ref_ptr<SoftVertexInfluenceMap> _softVertexInfluenceMap;

    };


    struct UpdateSoftRigGeometry : public osg::Drawable::UpdateCallback
    {
        UpdateSoftRigGeometry() {}

        UpdateSoftRigGeometry(const UpdateSoftRigGeometry& org, const osg::CopyOp& copyop):
            osg::Object(org, copyop),
            osg::Callback(org, copyop),
            osg::DrawableUpdateCallback(org, copyop) {}

        virtual void update(osg::NodeVisitor* nv, osg::Drawable* drw)
        {
            CGMSoftRigGeometry* geom = dynamic_cast<CGMSoftRigGeometry*>(drw);
            if(!geom)
                return;
            if(!geom->getSkeleton() && !geom->getParents().empty())
            {
                CGMSoftRigGeometry::FindNearestParentSoftSkeleton finder;
                if(geom->getParents().size() > 1)
                    osg::notify(osg::WARN) << "A SoftRigGeometry should not have multi parent ( " << geom->getName() << " )" << std::endl;
                geom->getParents()[0]->accept(finder);

                if(!finder._pSoftRoot.valid())
                {
                    osg::notify(osg::WARN) << "A SoftRigGeometry did not find a parent soft skeleton for SoftRigGeometry ( " << geom->getName() << " )" << std::endl;
                    return;
                }
                geom->getRigTransformImplementation()->prepareData(*geom);
                geom->setSkeleton(finder._pSoftRoot.get());
            }

            if(!geom->getSkeleton())
                return;

            if(geom->getNeedToComputeMatrix())
                geom->computeMatrixFromRootSkeleton();

            if(geom->getSourceGeometry())
            {
                osg::Drawable::UpdateCallback * up = dynamic_cast<osg::Drawable::UpdateCallback*>(geom->getSourceGeometry()->getUpdateCallback());
                if(up)
                    up->update(nv, geom->getSourceGeometry());
            }

            geom->update();
        }
    };
} // namespace GM
