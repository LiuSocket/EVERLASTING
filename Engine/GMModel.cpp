//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMModel.cpp
/// @brief		GMEngine - Model Module
/// @version	1.0
/// @author		LiuTao
/// @date		2024.10.12
//////////////////////////////////////////////////////////////////////////
#include "GMModel.h"
#include "GMMaterial.h"
#include "GMKit.h"
#include "GMAnimation.h"
#include "GMTangentSpaceGenerator.h"

#include <osg/CullFace>
#include <osg/StateSet>
#include <osg/Texture3D>
#include <osg/MatrixTransform>
#include <osg/ComputeBoundsVisitor>
#include <osg/PolygonOffset>
#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include <osgDB/ReadFile>

using namespace GM;

/*************************************************************************
Global Constants
*************************************************************************/

namespace GM
{
	class ComputeTangentVisitor : public osg::NodeVisitor
	{
	public:
		ComputeTangentVisitor(TraversalMode eMode = TRAVERSE_ALL_CHILDREN): osg::NodeVisitor(eMode) {}

		void apply(osg::Node& node) { traverse(node); }
		void apply(osg::Geode& node)
		{
			for (unsigned int i = 0; i < node.getNumDrawables(); ++i)
			{
				osg::Geometry* geom = dynamic_cast<osg::Geometry*>(node.getDrawable(i));
				if (geom) generateTangentArray(geom);
			}
			traverse(node);
		}
		void generateTangentArray(osg::Geometry* geom)
		{
			osg::ref_ptr<CGMTangentSpaceGenerator> tsg = new CGMTangentSpaceGenerator;
			tsg->generate(geom);
			geom->setVertexAttribArray(6, tsg->getTangentArray());
			geom->setVertexAttribBinding(6, osg::Geometry::BIND_PER_VERTEX);
			geom->setVertexAttribArray(7, tsg->getBinormalArray());
			geom->setVertexAttribBinding(7, osg::Geometry::BIND_PER_VERTEX);
		}
	};
}

/*************************************************************************
CGMModel Methods
*************************************************************************/
/** @brief 构造 */
CGMModel::CGMModel() :
	m_pKernelData(nullptr), m_pConfigData(nullptr), m_pCommonUniform(nullptr),
	m_pRootNode(nullptr), m_pAvatarNode(nullptr),
	m_pAnimationManager(nullptr), m_pMaterial(nullptr)
{
	// 创建动画管理器
	m_pAnimationManager = new CGMAnimation();
	// 创建材质管理器
	m_pMaterial = new CGMMaterial();
}

/** @brief 析构 */
CGMModel::~CGMModel()
{
	delete m_pAnimationManager;
	delete m_pMaterial;
}

/** @brief 初始化 */
bool CGMModel::Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData, CGMCommonUniform* pCommonUniform)
{
	m_pKernelData = pKernelData;
	m_pConfigData = pConfigData;
	m_pCommonUniform = pCommonUniform;

	m_pRootNode = new osg::Group;
	GM_Root->addChild(m_pRootNode.get());
	osg::StateSet* pStateset = m_pRootNode->getOrCreateStateSet();
	// 强制单面显示
	pStateset->setAttributeAndModes(new osg::CullFace(osg::CullFace::BACK), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	// 强制设置半透明混合模式
	pStateset->setAttributeAndModes(
		new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA),
		osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	pStateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	pStateset->setMode(GL_LIGHT0, osg::StateAttribute::ON);

	m_pMaterial->Init(pKernelData, pConfigData, pCommonUniform);

	m_pDDSOptions = new osgDB::Options("dds_flip");
	std::string strModelPath = m_pConfigData->strCorePath + m_strDefModelPath;
	// 加载背景模型
	m_pBackgroundNode = osgDB::readNodeFile(strModelPath + "Background.FBX", m_pDDSOptions);
	if (m_pBackgroundNode.valid())
	{
		m_pRootNode->addChild(m_pBackgroundNode.get());
		SGMModelData sData = SGMModelData();
		sData.strName = "Bacdground";
		sData.iEntRenderBin = 0;
		sData.eMaterial = EGM_MATERIAL_Background;
		// 设置材质
		_SetMaterial(m_pBackgroundNode.get(), sData);
	}

	// 加载角色模型
	m_pAvatarNode = osgDB::readNodeFile(strModelPath + "MIGI.FBX", m_pDDSOptions);
	if (m_pAvatarNode.valid())
	{
		m_pRootNode->addChild(m_pAvatarNode.get());
		SGMModelData sData = SGMModelData();
		sData.strName = "MIGI";
		sData.eMaterial = EGM_MATERIAL_PBR;
		// 设置材质
		_SetMaterial(m_pAvatarNode.get(), sData);
	}

	return true;
}

/** @brief 加载 */
bool CGMModel::Load()
{
	std::string strModelPath = m_pConfigData->strCorePath + m_strDefModelPath;
	// 加载背景模型
	if (m_pBackgroundNode.valid())
	{
		SGMModelData sData = SGMModelData();
		sData.strName = "Bacdground";
		sData.iEntRenderBin = 0;
		sData.eMaterial = EGM_MATERIAL_Background;
		// 设置材质
		_SetMaterial(m_pBackgroundNode.get(), sData);
	}
	
	// 加载角色模型
	if (m_pAvatarNode.valid())
	{
		SGMModelData sData = SGMModelData();
		sData.strName = "MIGI";
		sData.eMaterial = EGM_MATERIAL_PBR;
		// 设置材质
		_SetMaterial(m_pAvatarNode.get(), sData);
	}

	return true;
}

/** @brief 保存 */
bool CGMModel::Save()
{
	return true;
}

/** @brief 重置 */
bool CGMModel::Reset()
{
	m_pAnimationManager->Reset();
	return true;
}

/** @brief 更新 */
bool CGMModel::Update(double dDeltaTime)
{
	static double fConstantStep = 0.1;
	static double fDeltaStep = 0.0;
	if (fDeltaStep > fConstantStep)
	{
		_InnerUpdate(fDeltaStep);
		fDeltaStep = 0.0;
	}
	fDeltaStep += dDeltaTime;

	return true;
}

/** @brief 更新(在主相机更新之后) */
bool CGMModel::UpdatePost(double dDeltaTime)
{
	return true;
}

void CGMModel::_InnerUpdate(const double dDeltaTime)
{
}


bool CGMModel::_SetMaterial(osg::Node* pNode, const SGMModelData& sData)
{
	if (!pNode) return false;

	ComputeTangentVisitor ctv;
	pNode->accept(ctv);

	// 设置材质、
	switch (sData.eMaterial)
	{
	case EGM_MATERIAL_PBR:
	{
		m_pMaterial->SetPBRShader(pNode);
	}
	break;
	case EGM_MATERIAL_Background:
	{
		m_pMaterial->SetBackgroundShader(pNode);
	}
	break;
	default:
		break;
	}

	osg::StateSet* pStateSet = pNode->getOrCreateStateSet();
	// Blend
	switch (sData.eBlend)
	{
	case EGM_BLEND_Opaque:
	{
		pStateSet->setMode(GL_BLEND, osg::StateAttribute::OFF);
		pStateSet->setMode(GL_ALPHA_TEST, osg::StateAttribute::OFF);
	}
	break;
	case EGM_BLEND_Transparent:
	{
		pStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
		pStateSet->setMode(GL_ALPHA_TEST, osg::StateAttribute::OFF);
	}
	break;
	case EGM_BLEND_Cutoff:
	{
		pStateSet->setMode(GL_BLEND, osg::StateAttribute::OFF);
		pStateSet->setMode(GL_ALPHA_TEST, osg::StateAttribute::ON);

		osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
		alphaFunc->setFunction(osg::AlphaFunc::GEQUAL, 0.5f);
		pStateSet->setAttributeAndModes(alphaFunc, osg::StateAttribute::ON);
	}
	break;
	default:
		break;
	}
	pStateSet->setRenderBinDetails(sData.iEntRenderBin, "RenderBin");

	return true;
}

bool CGMModel::SetAnimationEnable(const std::string& strName, const bool bEnable)
{
	osg::Node* pNode = nullptr;// strName; to do
	if (!pNode) return false;

	if(bEnable)
		return m_pAnimationManager->AddAnimation(strName, pNode);
	else
		return m_pAnimationManager->RemoveAnimation(strName);
}

bool CGMModel::GetAnimationEnable(const std::string& strName)
{
	osg::Node* pNode = nullptr;// strName; to do
	if (!pNode) return false;

	return m_pAnimationManager->GetAnimationEnable(strName);
}

bool CGMModel::SetAnimationDuration(const std::string& strModelName, const float fDuration, const std::string& strAnimationName)
{
	return m_pAnimationManager->SetAnimationDuration(strModelName, fDuration, strAnimationName);
}

float CGMModel::GetAnimationDuration(const std::string& strModelName, const std::string& strAnimationName)
{
	return m_pAnimationManager->GetAnimationDuration(strModelName, strAnimationName);
}

bool CGMModel::SetAnimationMode(const std::string& strModelName, EGMPlayMode ePlayMode, const std::string& strAnimationName)
{
	return m_pAnimationManager->SetAnimationMode(strModelName, ePlayMode, strAnimationName);
}

EGMPlayMode CGMModel::GetAnimationMode(const std::string& strModelName, const std::string& strAnimationName)
{
	return m_pAnimationManager->GetAnimationMode(strModelName, strAnimationName);
}

bool CGMModel::SetAnimationPriority(const std::string& strModelName, const int iPriority, const std::string& strAnimationName)
{
	return m_pAnimationManager->SetAnimationPriority(strModelName, iPriority, strAnimationName);
}

int CGMModel::GetAnimationPriority(const std::string& strModelName, const std::string& strAnimationName)
{
	return m_pAnimationManager->GetAnimationPriority(strModelName, strAnimationName);
}

bool CGMModel::SetAnimationPlay(const std::string& strModelName, const float fWeight, const std::string& strAnimationName)
{
	return m_pAnimationManager->SetAnimationPlay(strModelName, fWeight, strAnimationName);
}

bool CGMModel::SetAnimationPause(const std::string& strModelName, const std::string& strAnimationName)
{
	return m_pAnimationManager->SetAnimationPause(strModelName, strAnimationName);
}

bool CGMModel::SetAnimationResume(const std::string& strModelName, const std::string& strAnimationName)
{
	return m_pAnimationManager->SetAnimationResume(strModelName, strAnimationName);
}