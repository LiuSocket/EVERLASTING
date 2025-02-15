#ifndef GMMRANIMATION_H
#define GMMRANIMATION_H

#include <fbxsdk.h>
#include <osgDB/ReaderWriter>
#include <osgAnimation/BasicAnimationManager>
#include "gmmMaterialToOsgStateSet.h"

namespace osgAnimation
{
    class AnimationManagerBase;
    class RigGeometry;
    class Skeleton;
}

typedef std::map< osg::Matrix, std::set<osgAnimation::RigGeometry*> > BindMatrixGeometryMap;
typedef std::map< FbxNode* , BindMatrixGeometryMap > BindMatrixMap;

class OsgFbxReader
{
public:
    FbxManager& pSdkManager;
    FbxScene& gmmScene;
    osg::ref_ptr<osgAnimation::AnimationManagerBase> pAnimationManager;
    FbxMaterialToOsgStateSet& gmmMaterialToOsgStateSet;
    std::map<FbxNode*, osg::Node*> nodeMap;
    BindMatrixMap boneBindMatrices;
    const std::set<const FbxNode*>& gmmSkeletons;
    std::map<FbxNode*, osgAnimation::Skeleton*> skeletonMap;
    const osgDB::Options& options;
    bool lightmapTextures, tessellatePolygons;

    enum AuthoringTool
    {
        UNKNOWN,
        OPENSCENEGRAPH,
        AUTODESK_3DSTUDIO_MAX
    } authoringTool;

    OsgFbxReader(
        FbxManager& pSdkManager1,
        FbxScene& gmmScene1,
        FbxMaterialToOsgStateSet& gmmMaterialToOsgStateSet1,
        const std::set<const FbxNode*>& gmmSkeletons1,
        const osgDB::Options& options1,
        AuthoringTool authoringTool1,
        bool lightmapTextures1,
        bool tessellatePolygons1)
        : pSdkManager(pSdkManager1),
        gmmScene(gmmScene1),
        gmmMaterialToOsgStateSet(gmmMaterialToOsgStateSet1),
        gmmSkeletons(gmmSkeletons1),
        options(options1),
        lightmapTextures(lightmapTextures1),
        tessellatePolygons(tessellatePolygons1),
        authoringTool(authoringTool1)
    {}

    osgDB::ReaderWriter::ReadResult readFbxNode(
        FbxNode*, bool& bIsBone, int& nLightCount);

    std::string readFbxAnimation(
        FbxNode*, const char* targetName);

    osgDB::ReaderWriter::ReadResult readFbxCamera(
        FbxNode* pNode);

    osgDB::ReaderWriter::ReadResult readFbxLight(
        FbxNode* pNode, int& nLightCount);

    osgDB::ReaderWriter::ReadResult readMesh(
        FbxNode* pNode, FbxMesh* fbxMesh,
        std::vector<StateSetContent>& stateSetList,
        const char* szName);

    osgDB::ReaderWriter::ReadResult readFbxMesh(
        FbxNode* pNode,
        std::vector<StateSetContent>&);
};

osgAnimation::Skeleton* getSkeleton(FbxNode*,
    const std::set<const FbxNode*>& gmmSkeletons,
    std::map<FbxNode*, osgAnimation::Skeleton*>&);

#endif
