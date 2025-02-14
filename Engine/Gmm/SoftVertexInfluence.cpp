//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		SoftVertexInfluenceMap.cpp
/// @brief		GMEngine - Soft SoftVertexInfluenceMap
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.13
//////////////////////////////////////////////////////////////////////////

#include "SoftVertexInfluence.h"
#include "GMSoftRigGeometry.h"
#include "SoftBoneMapVisitor.h"
#include <osg/Notify>
#include <iostream>
#include <algorithm>
#include <set>

using namespace GM;

struct invweight_ordered
{
    inline bool operator() (const osgAnimation::BoneWeight& bw1, const osgAnimation::BoneWeight& bw2) const
    {
        if (bw1.second > bw2.second)return true;
        if (bw1.second < bw2.second)return false;
        return(bw1.first < bw2.first);
    }
};

//Experimental Bone removal stuff
typedef std::vector<CGMSoftRigGeometry*> SoftRigList;
class CollectRigVisitor : public osg::NodeVisitor
{
public:
    META_NodeVisitor(osgAnimation, CollectRigVisitor)
    CollectRigVisitor();

    void apply(osg::Geometry& node);
    inline const SoftRigList& getRigList() const{return _map;}

protected:
    SoftRigList _map;
};

CollectRigVisitor::CollectRigVisitor() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

void CollectRigVisitor::apply(osg::Geometry& node)
{
    CGMSoftRigGeometry* rig = dynamic_cast<CGMSoftRigGeometry*>(&node);
    if ( rig )
        _map.push_back(rig);
}

bool recursiveisUsefull( CGMSoftBone* bone, std::set<std::string> foundnames)
{
    for(unsigned int i=0; i<bone->getNumChildren(); ++i)
    {
        CGMSoftBone* child = dynamic_cast<CGMSoftBone* >(bone->getChild(i));
        if(child)
        {
            if( foundnames.find(child->getName()) != foundnames.end() )
                return true;
            if( recursiveisUsefull(child,foundnames) ) 
                return true;
        }
    }
    return false;
}

void SoftVertexInfluenceMap::removeUnexpressedBones(CGMSoftSkeleton& skel) const
{
    CSoftBoneMapVisitor mapVisitor;
    skel.accept(mapVisitor);

    CollectRigVisitor rigvis;
    skel.accept(rigvis);

    SoftRigList rigs = rigvis.getRigList();
    SoftBoneMap boneMap = mapVisitor.getBoneMap();

    unsigned int removed=0;
    CGMSoftBone* child, *par;

    std::set<std::string> usebones;
    for(SoftRigList::iterator rigit = rigs.begin(); rigit != rigs.end(); ++rigit)
    {
        for(SoftVertexInfluenceMap::iterator mapit = (*rigit)->getInfluenceMap()->begin();
            mapit != (*rigit)->getInfluenceMap()->end();
            ++mapit)
        {
            usebones.insert((*mapit).first);
        }
    }
  
    for(SoftBoneMap::iterator bmit = boneMap.begin(); bmit != boneMap.end();)
    {
        if(usebones.find(bmit->second->getName()) == usebones.end())
        {
            if( !(par = bmit->second->getBoneParent()) )
            {
                ++bmit;
                continue;
            }

            CGMSoftBone* bone2rm = bmit->second.get();

            if( recursiveisUsefull(bone2rm,usebones))
            {
                ++bmit;
                continue;
            }

            ///Bone can be removed
            ++ removed;
            OSG_INFO<<"removing useless bone: "<< bone2rm->getName() <<std::endl;
            osg::NodeList nodes;

            for(unsigned int numchild = 0; numchild < bone2rm->getNumChildren(); numchild++)
            {
                if( (child = dynamic_cast<CGMSoftBone*>(bone2rm->getChild(numchild))) )
                {
                    if(par!=child &&child!=bone2rm)
                    {
                        par->addChild(child);
                        nodes.push_back(child);
                    }
                }
            }
            
            for(unsigned int i=0; i<nodes.size(); ++i)
            {
                bone2rm->removeChild(nodes[i]);
            }
            par->removeChild(bone2rm);

            ///rebuild bonemap after bone removal
            CSoftBoneMapVisitor mapVis ; 
            skel.accept(mapVis);
            boneMap = mapVis.getBoneMap();
            bmit = boneMap.begin(); 
                 
        }
        else 
        {
            ++bmit;
        }
    }
    OSG_WARN<<"Number of bone removed "<<removed<<std::endl;
}
