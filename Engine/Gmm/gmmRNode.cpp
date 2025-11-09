#include <cassert>
#include <memory>
#include <sstream>

#include <osg/io_utils>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/Material>
#include <osg/Texture2D>

#include <osgDB/FileNameUtils>
#include <osgDB/ReadFile>

#include "Animation/StackedSoftTransElement.h"
#include "Animation/StackedSoftRotElement.h"
#include "Animation/UpdateSoftBone.h"

#include <osgAnimation/Bone>
#include <osgAnimation/Skeleton>
#include <osgAnimation/AnimationManagerBase>
#include <osgAnimation/StackedMatrixElement>
#include <osgAnimation/StackedQuaternionElement>
#include <osgAnimation/StackedScaleElement>

#if defined(_MSC_VER)
	#pragma warning( disable : 4505 )
	#pragma warning( default : 4996 )
#endif
#include <fbxsdk.h>

#include "gmmReader.h"

bool isAnimated(FbxProperty& prop, FbxScene& gmmScene)
{
	for (int i = 0; i < gmmScene.GetSrcObjectCount<FbxAnimStack>(); ++i)
	{
		FbxAnimStack* pAnimStack = FbxCast<FbxAnimStack>(gmmScene.GetSrcObject<FbxAnimStack>(i));

		const int nbAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
		for (int j = 0; j < nbAnimLayers; j++)
		{
			FbxAnimLayer* pAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(j);
			if (prop.GetCurveNode(pAnimLayer, false))
			{
				return true;
			}
		}
	}
	return false;
}

bool isAnimated(FbxProperty& prop, const char* channel, FbxScene& gmmScene)
{
	for (int i = 0; i < gmmScene.GetSrcObjectCount<FbxAnimStack>(); ++i)
	{
		FbxAnimStack* pAnimStack = FbxCast<FbxAnimStack>(gmmScene.GetSrcObject<FbxAnimStack>(i));

		const int nbAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
		for (int j = 0; j < nbAnimLayers; j++)
		{
			FbxAnimLayer* pAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(j);
			if (prop.GetCurve(pAnimLayer, channel, false))
			{
				return true;
			}
		}
	}
	return false;
}

osg::Quat makeQuat(const FbxDouble3& degrees, EFbxRotationOrder gmmRotOrder)
{
	double radiansX = osg::DegreesToRadians(degrees[0]);
	double radiansY = osg::DegreesToRadians(degrees[1]);
	double radiansZ = osg::DegreesToRadians(degrees[2]);

	switch (gmmRotOrder)
	{
	case eEulerXYZ:
		return osg::Quat(
			radiansX, osg::Vec3d(1,0,0),
			radiansY, osg::Vec3d(0,1,0),
			radiansZ, osg::Vec3d(0,0,1));
	case eEulerXZY:
		return osg::Quat(
			radiansX, osg::Vec3d(1,0,0),
			radiansZ, osg::Vec3d(0,0,1),
			radiansY, osg::Vec3d(0,1,0));
	case eEulerYZX:
		return osg::Quat(
			radiansY, osg::Vec3d(0,1,0),
			radiansZ, osg::Vec3d(0,0,1),
			radiansX, osg::Vec3d(1,0,0));
	case eEulerYXZ:
		return osg::Quat(
			radiansY, osg::Vec3d(0,1,0),
			radiansX, osg::Vec3d(1,0,0),
			radiansZ, osg::Vec3d(0,0,1));
	case eEulerZXY:
		return osg::Quat(
			radiansZ, osg::Vec3d(0,0,1),
			radiansX, osg::Vec3d(1,0,0),
			radiansY, osg::Vec3d(0,1,0));
	case eEulerZYX:
		return osg::Quat(
			radiansZ, osg::Vec3d(0,0,1),
			radiansY, osg::Vec3d(0,1,0),
			radiansX, osg::Vec3d(1,0,0));
	case eSphericXYZ:
		{
			//I don't know what eSPHERIC_XYZ means, so this is a complete guess.
			osg::Quat quat;
			quat.makeRotate(osg::Vec3d(1.0, 0.0, 0.0), osg::Vec3d(degrees[0], degrees[1], degrees[2]));
			return quat;
		}
	default:
		OSG_WARN << "Invalid GMM rotation mode." << std::endl;
		return osg::Quat();
	}
}

void makeLocalMatrix(const FbxNode* pNode, osg::Matrix& m)
{
	/*From http://area.autodesk.com/forum/autodesk-gmm/fbx-sdk/the-makeup-of-the-local-matrix-of-an-kfbxnode/

	Local Matrix = LclTranslation * RotationOffset * RotationPivot *
	  PreRotation * LclRotation * PostRotation * RotationPivotInverse *
	  ScalingOffset * ScalingPivot * LclScaling * ScalingPivotInverse

	LocalTranslation : translate (xform -query -translation)
	RotationOffset: translation compensates for the change in the rotate pivot point (xform -q -rotateTranslation)
	RotationPivot: current rotate pivot position (xform -q -rotatePivot)
	PreRotation : joint orientation(pre rotation)
	LocalRotation: rotate transform (xform -q -rotation & xform -q -rotateOrder)
	PostRotation : rotate axis (xform -q -rotateAxis)
	RotationPivotInverse: inverse of RotationPivot
	ScalingOffset: translation compensates for the change in the scale pivot point (xform -q -scaleTranslation)
	ScalingPivot: current scale pivot position (xform -q -scalePivot)
	LocalScaling: scale transform (xform -q -scale)
	ScalingPivotInverse: inverse of ScalingPivot
	*/

	// When this flag is set to false, the RotationOrder, the Pre/Post rotation
	// values and the rotation limits should be ignored.
	bool rotationActive = pNode->RotationActive.Get();

	EFbxRotationOrder gmmRotOrder = rotationActive ? pNode->RotationOrder.Get() : eEulerXYZ;

	FbxDouble3 gmmLclPos = pNode->LclTranslation.Get();
	FbxDouble3 gmmRotOff = pNode->RotationOffset.Get();
	FbxDouble3 gmmRotPiv = pNode->RotationPivot.Get();
	FbxDouble3 gmmPreRot = pNode->PreRotation.Get();
	FbxDouble3 gmmLclRot = pNode->LclRotation.Get();
	FbxDouble3 gmmPostRot = pNode->PostRotation.Get();
	FbxDouble3 gmmSclOff = pNode->ScalingOffset.Get();
	FbxDouble3 gmmSclPiv = pNode->ScalingPivot.Get();
	FbxDouble3 gmmLclScl = pNode->LclScaling.Get();

	m.makeTranslate(osg::Vec3d(
		gmmLclPos[0] + gmmRotOff[0] + gmmRotPiv[0],
		gmmLclPos[1] + gmmRotOff[1] + gmmRotPiv[1],
		gmmLclPos[2] + gmmRotOff[2] + gmmRotPiv[2]));
	if (rotationActive)
	{
		m.preMultRotate(
			makeQuat(gmmPostRot, eEulerXYZ) *
			makeQuat(gmmLclRot, gmmRotOrder) *
			makeQuat(gmmPreRot, eEulerXYZ));
	}
	else
	{
		m.preMultRotate(makeQuat(gmmLclRot, gmmRotOrder));
	}
	m.preMultTranslate(osg::Vec3d(
		gmmSclOff[0] + gmmSclPiv[0] - gmmRotPiv[0],
		gmmSclOff[1] + gmmSclPiv[1] - gmmRotPiv[1],
		gmmSclOff[2] + gmmSclPiv[2] - gmmRotPiv[2]));
	m.preMultScale(osg::Vec3d(gmmLclScl[0], gmmLclScl[1], gmmLclScl[2]));
	m.preMultTranslate(osg::Vec3d(
		-gmmSclPiv[0],
		-gmmSclPiv[1],
		-gmmSclPiv[2]));
}

void readTranslationElement(FbxPropertyT<FbxDouble3>& prop,
							osgAnimation::UpdateMatrixTransform* pUpdate,
							osg::Matrix& staticTransform,
							FbxScene& gmmScene,
							const osg::Vec3& vSoftRange, const osg::Vec3& vSoftCenter)
{
	FbxDouble3 gmmPropValue = prop.Get();
	osg::Vec3d val(
		gmmPropValue[0],
		gmmPropValue[1],
		gmmPropValue[2]);

	if (isAnimated(prop, gmmScene))
	{
		if (!staticTransform.isIdentity())
		{
			pUpdate->getStackedTransforms().push_back(new osgAnimation::StackedMatrixElement(staticTransform));
			staticTransform.makeIdentity();
		}
		pUpdate->getStackedTransforms().push_back(new GM::StackedSoftTransElement(
			"translate", val, vSoftRange, vSoftCenter));
		//pUpdate->getStackedTransforms().push_back(new osgAnimation::StackedTranslateElement("translate", val));
	}
	else
	{
		staticTransform.preMultTranslate(val);
	}
}

void getRotationOrder(EFbxRotationOrder gmmRotOrder, int order[/*3*/])
{
	switch (gmmRotOrder)
	{
	case eEulerXZY:
		order[0] = 0; order[1] = 2; order[2] = 1;
		break;
	case eEulerYZX:
		order[0] = 1; order[1] = 2; order[2] = 0;
		break;
	case eEulerYXZ:
		order[0] = 1; order[1] = 0; order[2] = 2;
		break;
	case eEulerZXY:
		order[0] = 2; order[1] = 0; order[2] = 1;
		break;
	case eEulerZYX:
		order[0] = 2; order[1] = 1; order[2] = 0;
		break;
	default:
		order[0] = 0; order[1] = 1; order[2] = 2;
	}
}

void readRotationElement(FbxPropertyT<FbxDouble3>& prop,
						EFbxRotationOrder gmmRotOrder,
						bool quatInterpolate,
						osgAnimation::UpdateMatrixTransform* pUpdate,
						osg::Matrix& staticTransform,
						FbxScene& gmmScene,
						const double fShakeRange)
{
	if (isAnimated(prop, gmmScene))
	{
		if (quatInterpolate)
		{
			if (!staticTransform.isIdentity())
			{
				pUpdate->getStackedTransforms().push_back(
					new osgAnimation::StackedMatrixElement(staticTransform));
				staticTransform.makeIdentity();
			}
			pUpdate->getStackedTransforms().push_back(new osgAnimation::StackedQuaternionElement(
			    "quaternion", makeQuat(prop.Get(), gmmRotOrder)));
		}
		else
		{
			const char* curveNames[3] = {FBXSDK_CURVENODE_COMPONENT_X, FBXSDK_CURVENODE_COMPONENT_Y, FBXSDK_CURVENODE_COMPONENT_Z};
			osg::Vec3 axes[3] = {osg::Vec3(1,0,0), osg::Vec3(0,1,0), osg::Vec3(0,0,1)};

			FbxDouble3 gmmPropValue = prop.Get();
			gmmPropValue[0] = osg::DegreesToRadians(gmmPropValue[0]);
			gmmPropValue[1] = osg::DegreesToRadians(gmmPropValue[1]);
			gmmPropValue[2] = osg::DegreesToRadians(gmmPropValue[2]);

			int order[3] = {0, 1, 2};
			getRotationOrder(gmmRotOrder, order);

			for (int i = 0; i < 3; ++i)
			{
				int j = order[2-i];
				if (isAnimated(prop, curveNames[j], gmmScene))
				{
					if (!staticTransform.isIdentity())
					{
						pUpdate->getStackedTransforms().push_back(new osgAnimation::StackedMatrixElement(staticTransform));
						staticTransform.makeIdentity();
					}

					pUpdate->getStackedTransforms().push_back(new GM::StackedSoftRotElement(
						std::string("rotate") + curveNames[j], axes[j], gmmPropValue[j], fShakeRange));
					//pUpdate->getStackedTransforms().push_back(new osgAnimation::StackedRotateAxisElement(
					//	std::string("rotate") + curveNames[j], axes[j], gmmPropValue[j]));
				}
				else
				{
					staticTransform.preMultRotate(osg::Quat(gmmPropValue[j], axes[j]));
				}
			}
		}
	}
	else
	{
		staticTransform.preMultRotate(makeQuat(prop.Get(), gmmRotOrder));
	}
}

void readScaleElement(FbxPropertyT<FbxDouble3>& prop,
					  osgAnimation::UpdateMatrixTransform* pUpdate,
					  osg::Matrix& staticTransform,
					  FbxScene& gmmScene)
{
	FbxDouble3 gmmPropValue = prop.Get();
	osg::Vec3d val(
		gmmPropValue[0],
		gmmPropValue[1],
		gmmPropValue[2]);

	if (isAnimated(prop, gmmScene))
	{
		if (!staticTransform.isIdentity())
		{
			pUpdate->getStackedTransforms().push_back(new osgAnimation::StackedMatrixElement(staticTransform));
			staticTransform.makeIdentity();
		}
		pUpdate->getStackedTransforms().push_back(new osgAnimation::StackedScaleElement("scale", val));
	}
	else
	{
		staticTransform.preMultScale(val);
	}
}

void readUpdateMatrixTransform(osgAnimation::UpdateMatrixTransform* pUpdate, FbxNode* pNode, FbxScene& gmmScene)
{
	osg::Vec3 vSoftRange = osg::Vec3(0.0f, 0.0f, 0.0f);
	osg::Vec3 vSoftCenter = osg::Vec3(0.0f, 0.0f, 0.0f);
	double fElastic = 0.0;
	std::string name = pUpdate->getName();
	if (std::string::npos != name.find("Ribcage") ||
		std::string::npos != name.find("Spine"))
	{
		vSoftRange = osg::Vec3(0.4f, 0.01f, 0.01f);
		vSoftCenter = osg::Vec3(-0.5f, 0.0f, 0.0f);
		fElastic = 60.0;
	}
	else if (std::string::npos != name.find("Neck"))
	{
		vSoftRange = osg::Vec3(0.15f, 0.01f, 0.01f);
		vSoftCenter = osg::Vec3(0.0f, 0.0f, 0.0f);
		fElastic = 30.0;
	}
	else if (std::string::npos != name.find("arm"))
	{
		vSoftRange = osg::Vec3(0.2f, 0.03f, 0.03f);
		vSoftCenter = osg::Vec3(0.0f, 0.0f, 0.0f);
	}
	else {}

	osg::Matrix staticTransform;
	readTranslationElement(pNode->LclTranslation, pUpdate, staticTransform, gmmScene, vSoftRange, vSoftCenter);

	FbxDouble3 gmmRotOffset = pNode->RotationOffset.Get();
	FbxDouble3 gmmRotPiv = pNode->RotationPivot.Get();
	staticTransform.preMultTranslate(osg::Vec3d(
		gmmRotPiv[0] + gmmRotOffset[0],
		gmmRotPiv[1] + gmmRotOffset[1],
		gmmRotPiv[2] + gmmRotOffset[2]));

	// When this flag is set to false, the RotationOrder, the Pre/Post rotation
	// values and the rotation limits should be ignored.
	bool rotationActive = pNode->RotationActive.Get();

	EFbxRotationOrder gmmRotOrder = (rotationActive && pNode->RotationOrder.IsValid()) ?
		pNode->RotationOrder.Get() : eEulerXYZ;

	if (rotationActive)
	{
		staticTransform.preMultRotate(makeQuat(pNode->PreRotation.Get(), eEulerXYZ));
	}

	readRotationElement(pNode->LclRotation, gmmRotOrder,
		pNode->QuaternionInterpolate.IsValid() && pNode->QuaternionInterpolate.Get(),
		pUpdate, staticTransform, gmmScene, fElastic);

	if (rotationActive)
	{
		staticTransform.preMultRotate(makeQuat(pNode->PostRotation.Get(), eEulerXYZ));
	}

	FbxDouble3 gmmSclOffset = pNode->ScalingOffset.Get();
	FbxDouble3 gmmSclPiv = pNode->ScalingPivot.Get();
	staticTransform.preMultTranslate(osg::Vec3d(
		gmmSclOffset[0] + gmmSclPiv[0] - gmmRotPiv[0],
		gmmSclOffset[1] + gmmSclPiv[1] - gmmRotPiv[1],
		gmmSclOffset[2] + gmmSclPiv[2] - gmmRotPiv[2]));

	readScaleElement(pNode->LclScaling, pUpdate, staticTransform, gmmScene);

	staticTransform.preMultTranslate(osg::Vec3d(
		-gmmSclPiv[0],
		-gmmSclPiv[1],
		-gmmSclPiv[2]));

	if (!staticTransform.isIdentity())
	{
		pUpdate->getStackedTransforms().push_back(new osgAnimation::StackedMatrixElement(staticTransform));
	}
}

osg::Group* createGroupNode(FbxManager& pSdkManager, FbxNode* pNode,
	const std::string& animName, const osg::Matrix& localMatrix, bool bNeedSkeleton,
	std::map<FbxNode*, osg::Node*>& nodeMap, FbxScene& gmmScene)
{
	if (bNeedSkeleton)
	{
		osgAnimation::Bone* osgBone = new osgAnimation::Bone;
		osgBone->setDataVariance(osg::Object::DYNAMIC);
		osgBone->setName(pNode->GetName());
		GM::UpdateSoftBone* pUpdate = new GM::UpdateSoftBone(animName);
		readUpdateMatrixTransform(pUpdate, pNode, gmmScene);
		osgBone->setUpdateCallback(pUpdate);

		nodeMap.insert(std::pair<FbxNode*, osg::Node*>(pNode, osgBone));

		return osgBone;
	}
	else
	{
		bool bAnimated = !animName.empty();
		if (!bAnimated && localMatrix.isIdentity())
		{
			osg::Group* pGroup = new osg::Group;
			pGroup->setName(pNode->GetName());
			return pGroup;
		}

		osg::MatrixTransform* pTransform = new osg::MatrixTransform(localMatrix);
		pTransform->setName(pNode->GetName());

		if (bAnimated)
		{
			osgAnimation::UpdateMatrixTransform* pUpdate = new osgAnimation::UpdateMatrixTransform(animName);
			readUpdateMatrixTransform(pUpdate, pNode, gmmScene);
			pTransform->setUpdateCallback(pUpdate);
		}

		return pTransform;
	}
}

osgDB::ReaderWriter::ReadResult OsgFbxReader::readFbxNode(
	FbxNode* pNode,
	bool& bIsBone, int& nLightCount)
{
	if (FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute())
	{
		FbxNodeAttribute::EType attrType = lNodeAttribute->GetAttributeType();
		switch (attrType)
		{
		case FbxNodeAttribute::eNurbs:
		case FbxNodeAttribute::ePatch:
		case FbxNodeAttribute::eNurbsCurve:
		case FbxNodeAttribute::eNurbsSurface:
			{
				FbxGeometryConverter lConverter(&pSdkManager);
#if FBXSDK_VERSION_MAJOR < 2014
				if (!lConverter.TriangulateInPlace(pNode))
#else
				if (!lConverter.Triangulate(lNodeAttribute,true,false))
#endif
				{
					OSG_WARN << "Unable to triangulate GMM NURBS " << pNode->GetName() << std::endl;
				}
			}
			break;
		default:
			break;
		}
	}

	bIsBone = false;
	bool bCreateSkeleton = false;

	FbxNodeAttribute::EType lAttributeType = FbxNodeAttribute::eUnknown;
	if (pNode->GetNodeAttribute())
	{
		lAttributeType = pNode->GetNodeAttribute()->GetAttributeType();
		if (lAttributeType == FbxNodeAttribute::eSkeleton)
		{
			bIsBone = true;
		}
	}

	if (!bIsBone && gmmSkeletons.find(pNode) != gmmSkeletons.end())
	{
		bIsBone = true;
	}

	unsigned nMaterials = pNode->GetMaterialCount();
	std::vector<StateSetContent > stateSetList;

	for (unsigned i = 0; i < nMaterials; ++i)
	{
		FbxSurfaceMaterial* gmmMaterial = pNode->GetMaterial(i);
		assert(gmmMaterial);
		stateSetList.push_back(gmmMaterialToOsgStateSet.convert(gmmMaterial));
	}

	osg::NodeList skeletal, children;

	int nChildCount = pNode->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		FbxNode* pChildNode = pNode->GetChild(i);

		if (pChildNode->GetParent() != pNode)
		{
			//workaround for bug that occurs in some files exported from Blender
			continue;
		}

		bool bChildIsBone = false;
		osgDB::ReaderWriter::ReadResult childResult = readFbxNode(
			pChildNode, bChildIsBone, nLightCount);
		if (childResult.error())
		{
			return childResult;
		}
		else if (osg::Node* osgChild = childResult.getNode())
		{
			if (bChildIsBone)
			{
				if (!bIsBone) bCreateSkeleton = true;
				skeletal.push_back(osgChild);
			}
			else
			{
				children.push_back(osgChild);
			}
		}
	}

	std::string animName = readFbxAnimation(pNode, pNode->GetName());

	osg::Matrix localMatrix;
	makeLocalMatrix(pNode, localMatrix);
	bool bLocalMatrixIdentity = localMatrix.isIdentity();

	osg::ref_ptr<osg::Group> osgGroup;

	bool bEmpty = children.empty() && !bIsBone;

	switch (lAttributeType)
	{
	case FbxNodeAttribute::eMesh:
		{
			size_t bindMatrixCount = boneBindMatrices.size();
			osgDB::ReaderWriter::ReadResult meshRes = readFbxMesh(pNode, stateSetList);
			if (meshRes.error())
			{
				return meshRes;
			}
			else if (osg::Node* node = meshRes.getNode())
			{
				bEmpty = false;

				if (bindMatrixCount != boneBindMatrices.size())
				{
					//The mesh is skinned therefore the bind matrix will handle all transformations.
					localMatrix.makeIdentity();
					bLocalMatrixIdentity = true;
				}

				if (animName.empty() &&
					children.empty() &&
					skeletal.empty() &&
					bLocalMatrixIdentity)
				{
					return osgDB::ReaderWriter::ReadResult(node);
				}

				children.insert(children.begin(), node);
			}
		}
		break;
	case FbxNodeAttribute::eCamera:
	case FbxNodeAttribute::eLight:
		{
			osgDB::ReaderWriter::ReadResult res =
				lAttributeType == FbxNodeAttribute::eCamera ?
				readFbxCamera(pNode) : readFbxLight(pNode, nLightCount);
			if (res.error())
			{
				return res;
			}
			else if (osg::Group* resGroup = dynamic_cast<osg::Group*>(res.getObject()))
			{
				bEmpty = false;
				if (animName.empty() &&
					bLocalMatrixIdentity)
				{
					osgGroup = resGroup;
				}
				else
				{
					children.insert(children.begin(), resGroup);
				}
			}
		}
		break;
	default:
		break;
	}

	if (bEmpty)
	{
		osgDB::ReaderWriter::ReadResult(0);
	}

	if (!osgGroup) osgGroup = createGroupNode(pSdkManager, pNode, animName, localMatrix, bIsBone, nodeMap, gmmScene);

	osg::Group* pAddChildrenTo = osgGroup.get();
	if (bCreateSkeleton)
	{
		osgAnimation::Skeleton* osgSkeleton = getSkeleton(pNode, gmmSkeletons, skeletonMap);
		osgSkeleton->setDefaultUpdateCallback();
		pAddChildrenTo->addChild(osgSkeleton);
		pAddChildrenTo = osgSkeleton;
	}

	for (osg::NodeList::iterator it = skeletal.begin(); it != skeletal.end(); ++it)
	{
		pAddChildrenTo->addChild(it->get());
	}
	for (osg::NodeList::iterator it = children.begin(); it != children.end(); ++it)
	{
		pAddChildrenTo->addChild(it->get());
	}


	return osgDB::ReaderWriter::ReadResult(osgGroup.get());
}

osgAnimation::Skeleton* getSkeleton(FbxNode* gmmNode,
	const std::set<const FbxNode*>& gmmSkeletons,
	std::map<FbxNode*, osgAnimation::Skeleton*>& skeletonMap)
{
	//Find the first non-skeleton ancestor of the node.
	while (gmmNode &&
		((gmmNode->GetNodeAttribute() &&
		gmmNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) ||
		gmmSkeletons.find(gmmNode) != gmmSkeletons.end()))
	{
		gmmNode = gmmNode->GetParent();
	}

	std::map<FbxNode*, osgAnimation::Skeleton*>::const_iterator it = skeletonMap.find(gmmNode);
	if (it == skeletonMap.end())
	{
		osgAnimation::Skeleton* skel = new osgAnimation::Skeleton;
		skel->setDefaultUpdateCallback();
		skeletonMap.insert(std::pair<FbxNode*, osgAnimation::Skeleton*>(gmmNode, skel));
		return skel;
	}
	else
	{
		return it->second;
	}
}
