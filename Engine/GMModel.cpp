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

#include <osg/StateSet>
#include <osg/Texture3D>
#include <osg/MatrixTransform>
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
/** @brief ���� */
CGMModel::CGMModel()
{
    // �������ʹ�����
    m_pMaterial = new CGMMaterial();
}

/** @brief ���� */
CGMModel::~CGMModel()
{
    delete m_pMaterial;
}

/** @brief ��ʼ�� */
bool CGMModel::Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData)
{
    m_pKernelData = pKernelData;
    m_pConfigData = pConfigData;

    m_pRootNode = new osg::Group;
    GM_Root->addChild(m_pRootNode.get());

    //������Ӱ
    GM_LIGHT.SetShadowEnable(m_pRootNode);
    unsigned int iValue = osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE;
    osg::ref_ptr<osg::StateSet> pStateset = m_pRootNode->getOrCreateStateSet();
    pStateset->setTextureAttributeAndModes(SHADOW_TEX_UNIT, GM_LIGHT.GetShadowMap(), iValue);
    pStateset->addUniform(new osg::Uniform("texShadow", SHADOW_TEX_UNIT), iValue);
    pStateset->addUniform(GM_LIGHT.GetView2ShadowMatrixUniform());

    // ǿ�����ð�͸�����ģʽ
    pStateset->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA),iValue);

    m_pMaterial->Init(pKernelData, pConfigData);
    m_pDDSOptions = new osgDB::Options("dds_flip");
    return true;
}

/** @brief ���� */
bool CGMModel::Load()
{
    for (auto& itr : m_pNodeMap)
    {
        _SetMaterial(itr.second.get(), m_pModelDataMap.at(itr.first));
    }

    return true;
}

/** @brief ���� */
bool CGMModel::Save()
{
    return true;
}

/** @brief ���� */
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
    return true;
}

bool CGMModel::Add(const SGMModelData& sData)
{
    // ���ر���ģ��
    osg::ref_ptr<osg::Node> pNode = osgDB::readNodeFile(m_pConfigData->strCorePath + m_strDefModelPath + sData.strFilePath,
        m_pDDSOptions);// ��֤dds�������ȷ����
    if (pNode.valid())
    {
        // ������Ӱ
        if (sData.bCastShadow)
            pNode->setNodeMask(GM_MAIN_MASK | GM_SHADOW_CAST_MASK);
        else
            pNode->setNodeMask(GM_MAIN_MASK);

        if(!m_pRootNode->containsNode(pNode.get()))
            m_pRootNode->addChild(pNode.get());

        m_pModelDataMap[sData.strName] = sData;
        m_pNodeMap[sData.strName] = pNode;
        // ���ò���
        _SetMaterial(pNode.get(), sData);
        return true;
    }
    return false;
}

bool CGMModel::_SetMaterial(osg::Node* pNode, const SGMModelData& sData)
{
    if (!pNode) return false;

    ComputeTangentVisitor ctv;
    pNode->accept(ctv);

    // ���ò���
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
    // ������Ӱ
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

osg::Node* CGMModel::GetNode(const std::string& strModelName)
{
    return _GetNode(strModelName);
}

osg::Node* CGMModel::_GetNode(const std::string& strModelName)
{
    if (m_pNodeMap.end() != m_pNodeMap.find(strModelName))
    {
        return m_pNodeMap.at(strModelName).get();
    }
    return nullptr;
}

void CGMModel::_InnerUpdate(const double dDeltaTime)
{
}