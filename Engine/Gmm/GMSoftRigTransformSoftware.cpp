//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		CGMSoftRigTransformSoftware.cpp
/// @brief		GMEngine - Soft CGMSoftRigTransformSoftware
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.12
//////////////////////////////////////////////////////////////////////////

#include "GMSoftRigGeometry.h"
#include "GMSoftRigTransformSoftware.h"
#include "SoftBoneMapVisitor.h"
#include "SoftVertexInfluence.h"

#include <algorithm>

using namespace GM;

CGMSoftRigTransformSoftware::CGMSoftRigTransformSoftware() : CGMSoftRigTransform()
{
    _needInit = true;
}

CGMSoftRigTransformSoftware::CGMSoftRigTransformSoftware(const CGMSoftRigTransformSoftware& rts,const osg::CopyOp& copyop)
    : CGMSoftRigTransform(rts, copyop),
    _needInit(rts._needInit),
    _invalidInfluence(rts._invalidInfluence)
{
}

void CGMSoftRigTransformSoftware::buildMinimumUpdateSet( const CGMSoftRigGeometry& rig )
{
    ///1 Create Index2Vec<BoneWeight>
    unsigned int nbVertices=rig.getSourceGeometry()->getVertexArray()->getNumElements();
    const SoftVertexInfluenceMap &vertexInfluenceMap = *rig.getInfluenceMap();
    std::vector<SoftBonePtrWeightList> perVertexInfluences;
    perVertexInfluences.reserve(nbVertices);
    perVertexInfluences.resize(nbVertices);

    unsigned int vimapBoneID = 0;
    for (SoftVertexInfluenceMap::const_iterator perBoneinfit = vertexInfluenceMap.begin();
            perBoneinfit != vertexInfluenceMap.end();
            ++perBoneinfit,++vimapBoneID)
    {
        const osgAnimation::IndexWeightList& inflist = perBoneinfit->second;
        const std::string& bonename = perBoneinfit->first;

        if (bonename.empty())
        {
            OSG_WARN << "CGMSoftRigTransformSoftware::SoftVertexInfluenceMap contains unnamed bone IndexWeightList" << std::endl;
        }
        for(osgAnimation::IndexWeightList::const_iterator infit = inflist.begin(); infit!=inflist.end(); ++infit)
        {
            const osgAnimation::VertexIndexWeight &iw = *infit;
            const unsigned int &index = iw.first;
            float weight = iw.second;
            perVertexInfluences[index].push_back(SoftBonePtrWeight(vimapBoneID, weight));
        }
    }

    ///2 Create inverse mapping Vec<BoneWeight>2Vec<Index> from previous built Index2Vec<BoneWeight>
    ///in order to minimize weighted matrices computation on update
    _uniqVertexGroupList.clear();

    typedef std::map<SoftBonePtrWeightList, SoftVertexGroup> UnifyBoneGroup;
    UnifyBoneGroup unifyBuffer;
    unsigned int vertexID = 0;
    for (std::vector<SoftBonePtrWeightList>::iterator perVertinfit = perVertexInfluences.begin();
            perVertinfit!=perVertexInfluences.end();
            ++perVertinfit,++vertexID)
    {
        SoftBonePtrWeightList &boneinfs = *perVertinfit;
        // sort the vector to have a consistent key
        std::sort(boneinfs.begin(), boneinfs.end() );
        // we use the vector<BoneWeight> as key to differentiate group
        UnifyBoneGroup::iterator result = unifyBuffer.find(boneinfs);
        if (result != unifyBuffer.end())
            result->second.getVertices().push_back(vertexID);
        else
        {
            SoftVertexGroup& vg = unifyBuffer[boneinfs];
            vg.getBoneWeights() = boneinfs;
            vg.getVertices().push_back(vertexID);
        }
    }
    
    _uniqVertexGroupList.reserve(unifyBuffer.size());
    for (UnifyBoneGroup::const_iterator it = unifyBuffer.begin(); it != unifyBuffer.end(); ++it)
    {
        _uniqVertexGroupList.push_back(it->second);
    }
    OSG_INFO << "uniq groups " << _uniqVertexGroupList.size() << " for " << rig.getName() << std::endl;
}


bool CGMSoftRigTransformSoftware::prepareData(CGMSoftRigGeometry&rig)
{
    ///set geom as it source
    if (rig.getSourceGeometry())
        rig.copyFrom(*rig.getSourceGeometry());

    osg::Vec3Array* normalSrc = dynamic_cast<osg::Vec3Array*>(rig.getSourceGeometry()->getNormalArray());
    osg::Vec3Array* positionSrc = dynamic_cast<osg::Vec3Array*>(rig.getSourceGeometry()->getVertexArray());

    if(!(positionSrc) || positionSrc->empty() )
        return false;
    if(normalSrc && normalSrc->size() != positionSrc->size())
        return false;

    /// setup Vertex and Normal arrays with copy of sources
    rig.setVertexArray(new osg::Vec3Array);
    osg::Vec3Array* positionDst = new osg::Vec3Array;
    rig.setVertexArray(positionDst);
    *positionDst = *positionSrc;
    positionDst->setDataVariance(osg::Object::DYNAMIC);

    if(normalSrc)
    {
        osg::Vec3Array* normalDst = new osg::Vec3Array;
        *normalDst = *normalSrc;
        rig.setNormalArray(normalDst, osg::Array::BIND_PER_VERTEX);
        normalDst->setDataVariance(osg::Object::DYNAMIC);
    }

    /// build minimal set of SoftVertexGroup
    buildMinimumUpdateSet(rig);

    return true;
}

bool CGMSoftRigTransformSoftware::init(CGMSoftRigGeometry&rig)
{
    ///test if dataprepared
    if(_uniqVertexGroupList.empty())
    {
        prepareData(rig);
        return false;
    }

    if(!rig.getSkeleton())
        return false;
    
    ///get bonemap from skeleton
    CSoftBoneMapVisitor mapVisitor;
    rig.getSkeleton()->accept(mapVisitor);
    SoftBoneMap boneMap = mapVisitor.getBoneMap();
    SoftVertexInfluenceMap & vertexInfluenceMap = *rig.getInfluenceMap();

    ///create local bonemap
    std::vector<CGMSoftBone*> localid2bone;
    localid2bone.reserve(vertexInfluenceMap.size());
    for (SoftVertexInfluenceMap::const_iterator perBoneinfit = vertexInfluenceMap.begin();
            perBoneinfit != vertexInfluenceMap.end();
            ++perBoneinfit)
    {
        const std::string& bonename = perBoneinfit->first;

        if (bonename.empty())
        {
            OSG_WARN << "CGMSoftRigTransformSoftware::VertexInfluenceMap contains unnamed bone IndexWeightList" << std::endl;
        }
        SoftBoneMap::const_iterator bmit = boneMap.find(bonename);
        if (bmit == boneMap.end() )
        {
            if (_invalidInfluence.find(bonename) == _invalidInfluence.end())
            {
                _invalidInfluence[bonename] = true;
                OSG_WARN << "CGMSoftRigTransformSoftware CGMSoftBone " << bonename << " not found, skip the influence group " << std::endl;
            }

            localid2bone.push_back(0);
            continue;
        }
        
        CGMSoftBone* bone = bmit->second.get();
        localid2bone.push_back(bone);
    }

    ///fill bone ptr in the _uniqVertexGroupList
    for(SoftVertexGroupList::iterator itvg = _uniqVertexGroupList.begin(); itvg != _uniqVertexGroupList.end(); ++itvg)
    {
        SoftVertexGroup& uniq = *itvg;
        for(SoftBonePtrWeightList::iterator bwit = uniq.getBoneWeights().begin(); bwit != uniq.getBoneWeights().end(); )
        {
            CGMSoftBone * b = localid2bone[bwit->getBoneID()];
            if(!b)
                bwit = uniq.getBoneWeights().erase(bwit);
            else
                bwit++->setBonePtr(b);
        }
    }

    for(SoftVertexGroupList::iterator itvg = _uniqVertexGroupList.begin(); itvg != _uniqVertexGroupList.end(); ++itvg)
    {
        itvg->normalize();
    }

    _needInit = false;

    return true;
}

void CGMSoftRigTransformSoftware::SoftVertexGroup::normalize()
{
    osg::Matrix::value_type sum=0;
    for(SoftBonePtrWeightList::iterator bwit = _boneweights.begin(); bwit != _boneweights.end(); ++bwit )
    {
        sum += bwit->getWeight();
    }

    if (sum < 1e-4)
    {
        OSG_WARN << "CGMSoftRigTransformSoftware::SoftVertexGroup: warning try to normalize a zero sum vertexgroup" << std::endl;
    }
    else
    {
        for(SoftBonePtrWeightList::iterator bwit = _boneweights.begin(); bwit != _boneweights.end(); ++bwit )
        {
            bwit->setWeight(bwit->getWeight()/sum);
        }
    }
}

void CGMSoftRigTransformSoftware::operator()(CGMSoftRigGeometry& geom)
{
    if (_needInit && !init(geom)) return;

    if (!geom.getSourceGeometry())
    {
        OSG_WARN << this << " CGMSoftRigTransformSoftware no source geometry found on CGMSoftRigGeometry" << std::endl;
        return;
    }
    
    osg::Geometry& source = *geom.getSourceGeometry();
    osg::Geometry& destination = geom;

    osg::Vec3Array* positionSrc = static_cast<osg::Vec3Array*>(source.getVertexArray());
    osg::Vec3Array* positionDst = static_cast<osg::Vec3Array*>(destination.getVertexArray());
    osg::Vec3Array* normalSrc = dynamic_cast<osg::Vec3Array*>(source.getNormalArray());
    osg::Vec3Array* normalDst = static_cast<osg::Vec3Array*>(destination.getNormalArray());


    compute<osg::Vec3>(geom.getMatrixFromSkeletonToGeometry(),
                       geom.getInvMatrixFromSkeletonToGeometry(),
                       &positionSrc->front(),
                       &positionDst->front());    
    positionDst->dirty();

    if (normalSrc )
    {
        computeNormal<osg::Vec3>(geom.getMatrixFromSkeletonToGeometry(),
                                 geom.getInvMatrixFromSkeletonToGeometry(),
                                 &normalSrc->front(),
                                 &normalDst->front());
        normalDst->dirty();
    }

}
