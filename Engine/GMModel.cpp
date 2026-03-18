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
#include "GMCommonUniform.h"
#include "GMLight.h"
#include "GMKit.h"
#include "GMTangentSpaceGenerator.h"
#include "Animation/GMAnimation.h"
#include "Cipher/HydroCipher.h"

#include <osg/StateSet>
#include <osg/Texture3D>
#include <osg/PositionAttitudeTransform>
#include <osg/ComputeBoundsVisitor>
#include <osg/PolygonOffset>
#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include <osg/CullFace>
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
CGMModel::CGMModel()
{
	// 创建材质管理器
	m_pMaterial = new CGMMaterial();
}

/** @brief 析构 */
CGMModel::~CGMModel()
{
	delete m_pMaterial;
}

/** @brief 初始化 */
bool CGMModel::Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData)
{
	m_pKernelData = pKernelData;
	m_pConfigData = pConfigData;

	m_pRootNode = new osg::Group;
	GM_Root->addChild(m_pRootNode.get());

	//设置阴影
	GM_LIGHT.SetCastShadowEnable(m_pRootNode, true);
	unsigned int iValue = osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE;
	osg::ref_ptr<osg::StateSet> pStateset = m_pRootNode->getOrCreateStateSet();
	pStateset->setTextureAttributeAndModes(SHADOW_TEX_UNIT, GM_LIGHT.GetShadowMap(), iValue);
	pStateset->addUniform(new osg::Uniform("texShadow", SHADOW_TEX_UNIT), iValue);
	pStateset->addUniform(GM_LIGHT.GetView2ShadowMatrixUniform());

	// 强制设置半透明混合模式
	pStateset->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA),iValue);

	m_pMaterial->Init(pKernelData, pConfigData);
	m_pDDSOptions = new osgDB::Options("dds_flip");
	return true;
}

/** @brief 加载 */
bool CGMModel::Load()
{
	for (auto& itr : m_pTransMap)
	{
		_SetMaterial(itr.second->getChild(0), m_pModelDataMap.at(itr.first));
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
	GM_ANIMATION.Reset();
	return true;
}

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

bool CGMModel::UpdatePost(double dDeltaTime)
{
	m_pMaterial->UpdatePost(dDeltaTime);
	return true;
}

bool CGMModel::Add(const SGMModelData& sData)
{
	if (m_pModelDataMap.end() != m_pModelDataMap.find(sData.strName))
	{
		// 如果已经存在则不再添加
		return false;
	}

	std::string strRealFilePath = m_pConfigData->strCorePath + m_strDefModelPath + sData.strFilePath;
	// 如果是CIP文件则使用HydroCipher解密
	if (sData.strFilePath.find(".CIP") != std::string::npos)
	{
		// CIP文件需要解密
		auto cipher = HydroCipher::create(AlgorithmType::XOR);
		std::vector<uint8_t> key(32, 0xD5C9);

		std::string strCipherFilePath = strRealFilePath;
		strRealFilePath = m_pConfigData->strCorePath + m_strDefModelPath + sData.strName + ".GMM";
		if (!cipher->decrypt(strCipherFilePath, strRealFilePath, key))
		{
			return false; // 解密失败
		}
	}
	// 加载模型
	osg::ref_ptr<osg::Node> pNode = osgDB::readNodeFile(strRealFilePath, m_pDDSOptions);// 保证dds纹理的正确加载
	if (pNode.valid())
	{
		osg::ref_ptr<osg::PositionAttitudeTransform> pTransform = new osg::PositionAttitudeTransform;
		// 设置模型的初始位置、旋转和缩放
		pTransform->setAttitude(osg::Quat(
			osg::DegreesToRadians(sData.vOri.x), osg::Vec3f(1, 0, 0),
			osg::DegreesToRadians(sData.vOri.y), osg::Vec3f(0, 1, 0),
			osg::DegreesToRadians(sData.vOri.z), osg::Vec3f(0, 0, 1)));
		pTransform->setPosition(osg::Vec3f(sData.vPos.x, sData.vPos.y, sData.vPos.z));
		pTransform->setScale(osg::Vec3f(sData.vScale.x, sData.vScale.y, sData.vScale.z));

		pTransform->addChild(pNode.get());
		// 设置阴影
		if (sData.bCastShadow)
			pTransform->setNodeMask(GM_MAIN_MASK | GM_SHADOW_CAST_MASK);
		else
			pTransform->setNodeMask(GM_MAIN_MASK);

		if(!m_pRootNode->containsNode(pTransform.get()))
			m_pRootNode->addChild(pTransform.get());

		m_pModelDataMap[sData.strName] = sData;
		m_pTransMap[sData.strName] = pTransform;

		// 设置材质
		_SetMaterial(pTransform.get(), sData);

		// 如果是加密文件且加载成功，则删除解密后的模型文件，防止泄露
		if (sData.strFilePath.find(".CIP") != std::string::npos)
		{
			if (std::remove(strRealFilePath.c_str()) != 0)
			{
				return false; // 删除失败
			}
		}
		return true;
	}
	return false;
}

bool CGMModel::Edit(const std::string& strOldName, SGMModelData& sNewData)
{
	// 如果不存在则无法修改
	if (m_pModelDataMap.end() == m_pModelDataMap.find(strOldName))
		return false;

	// 如果修改了名称且新名称已经存在则无法修改
	if (strOldName != sNewData.strName && m_pModelDataMap.end() != m_pModelDataMap.find(sNewData.strName))
		return false;

	osg::Node* pNode = _GetNode(strOldName);
	if (!pNode) return false;
	
	if (m_pModelDataMap[strOldName].vPos != sNewData.vPos ||
		m_pModelDataMap[strOldName].vOri != sNewData.vOri ||
		m_pModelDataMap[strOldName].vScale != sNewData.vScale)
	{
		// 修改位置、旋转和缩放
		osg::ref_ptr<osg::PositionAttitudeTransform> pTransform = m_pTransMap[strOldName].get();
		pTransform->setAttitude(osg::Quat(
			osg::DegreesToRadians(sNewData.vOri.x), osg::Vec3f(1, 0, 0),
			osg::DegreesToRadians(sNewData.vOri.y), osg::Vec3f(0, 1, 0),
			osg::DegreesToRadians(sNewData.vOri.z), osg::Vec3f(0, 0, 1)));
		pTransform->setPosition(osg::Vec3f(sNewData.vPos.x, sNewData.vPos.y, sNewData.vPos.z));
		pTransform->setScale(osg::Vec3f(sNewData.vScale.x, sNewData.vScale.y, sNewData.vScale.z));
	}
	if (m_pModelDataMap[strOldName].eMaterial != sNewData.eMaterial)
	{
		// 修改材质
		_SetMaterial(m_pTransMap[strOldName].get(), sNewData);
	}

	// 如果修改了名称则更新map
	if (strOldName != sNewData.strName)
	{
		m_pModelDataMap.erase(strOldName);
		m_pModelDataMap[sNewData.strName] = sNewData;

		m_pTransMap[sNewData.strName] = m_pTransMap[strOldName].get();
		m_pTransMap.erase(strOldName);
	}
	else
	{
		m_pModelDataMap[strOldName] = sNewData;
	}
	return true;
}

void CGMModel::ResizeScreen(const int width, const int height)
{
	m_pMaterial->ResizeScreen(width, height);
}

bool CGMModel::_SetMaterial(osg::Node* pNode, const SGMModelData& sData)
{
	if (!pNode) return false;

	ComputeTangentVisitor ctv;
	pNode->accept(ctv);

	// 设置材质
	switch (sData.eMaterial)
	{
	case EGM_MATERIAL_PBR:
	{
		m_pMaterial->SetPBRMaterial(pNode);
	}
	break;
	case EGM_MATERIAL_Human:
	{
		m_pMaterial->SetHumanMaterial(pNode);
		m_pMaterial->GetEyeTransform(m_pEyeTransVector);
	}
	break;
	case EGM_MATERIAL_SSS:
	{
		m_pMaterial->SetSSSMaterial(pNode);
	}
	break;
	case EGM_MATERIAL_Eye:
	{
		m_pMaterial->SetEyeMaterial(pNode);
	}
	break;
	case EGM_MATERIAL_Background:
	{
		m_pMaterial->SetBackgroundMaterial(pNode);
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
	pStateSet->setAttributeAndModes(new osg::CullFace(), osg::StateAttribute::ON);
	// 设置阴影
	pStateSet->setDefine("SHADOW_RECEIVE", osg::StateAttribute::ON);

	return true;
}

bool CGMModel::SetAnimationEnable(const std::string& strName, const bool bEnable)
{
	osg::Node* pNode = _GetNode(strName);
	if (!pNode) return false;

	if(bEnable)
		return GM_ANIMATION.AddAnimation(strName, pNode);
	else
		return GM_ANIMATION.RemoveAnimation(strName);
}

bool CGMModel::GetAnimationEnable(const std::string& strName)
{
	osg::Node* pNode = _GetNode(strName);
	if (!pNode) return false;

	return GM_ANIMATION.GetAnimationEnable(strName);
}

SGMModelData CGMModel::GetModelData(const std::string& strName) const
{
	if (m_pModelDataMap.end() != m_pModelDataMap.find(strName))
	{
		return m_pModelDataMap.at(strName);
	}
	return SGMModelData();
}

osg::PositionAttitudeTransform* CGMModel::GetPositionAttitudeTransform(const std::string& strName) const
{
	if (m_pTransMap.end() != m_pTransMap.find(strName))
	{
		return m_pTransMap.at(strName).get();
	}
	return nullptr;
}

osg::Node* CGMModel::_GetNode(const std::string& strName) const
{
	if (m_pTransMap.end() != m_pTransMap.find(strName))
	{
		return m_pTransMap.at(strName)->getChild(0);
	}
	return nullptr;
}

void CGMModel::_InnerUpdate(const double dDeltaTime)
{
}