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
#include "GMAnimation.h"
#include "GMTangentSpaceGenerator.h"

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
CGMModel::CGMModel()
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
bool CGMModel::Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData)
{
    m_pKernelData = pKernelData;
    m_pConfigData = pConfigData;

    m_pRootNode = new osg::Group;
    GM_Root->addChild(m_pRootNode.get());

    //设置阴影
    GM_LIGHT.SetShadowEnable(m_pRootNode);
    unsigned int iValue = osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE;
    osg::ref_ptr<osg::StateSet> pStateset = m_pRootNode->getOrCreateStateSet();
    pStateset->setTextureAttributeAndModes(SHADOW_TEX_UNIT, GM_LIGHT.GetShadowMap(), iValue);
    pStateset->addUniform(new osg::Uniform("texShadow", SHADOW_TEX_UNIT), iValue);
    pStateset->addUniform(GM_LIGHT.GetView2ShadowMatrixUniform());

    // 强制设置半透明混合模式
    pStateset->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA),iValue);

    m_pMaterial->Init(pKernelData, pConfigData);

    m_pDDSOptions = new osgDB::Options("dds_flip");

    // 加载背景模型
    SGMModelData sData = SGMModelData();
    sData.strName = "Bacdground";
    sData.strFilePath = "Background.FBX";
    sData.iEntRenderBin = 0;
    sData.eMaterial = EGM_MATERIAL_Background;
    sData.bCastShadow = false;
    Add(sData);

    // 加载角色模型
    SGMModelData sData1 = SGMModelData();
    sData1.strName = "MIGI";
    sData1.strFilePath = "MIGI.FBX";
    sData1.eMaterial = EGM_MATERIAL_Human;
    Add(sData1);

    SetAnimationEnable("MIGI", true);

    SetAnimationMode("MIGI", EGM_PLAY_LOOP, "bone_idle");
    SetAnimationPriority("MIGI", 200, "bone_idle");
    SetAnimationPlay("MIGI", 1.0f, "bone_idle");

    SetAnimationMode("MIGI", EGM_PLAY_LOOP, "eye_blink");
    SetAnimationPriority("MIGI", 100, "eye_blink");
    SetAnimationPlay("MIGI", 1.0f, "eye_blink");

    return true;
}

/** @brief 加载 */
bool CGMModel::Load()
{
    for (auto& itr : m_pNodeMap)
    {
        _SetMaterial(itr.second.get(), m_pModelDataMap.at(itr.first));
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
    // 加载背景模型
    osg::ref_ptr<osg::Node> pNode = osgDB::readNodeFile(m_pConfigData->strCorePath + m_strDefModelPath + sData.strFilePath,
        m_pDDSOptions);// 保证dds纹理的正确加载
    if (pNode.valid())
    {
        // 设置阴影
        if (sData.bCastShadow)
            pNode->setNodeMask(GM_MAIN_MASK | GM_SHADOW_CAST_MASK);
        else
            pNode->setNodeMask(GM_MAIN_MASK);

        if(!m_pRootNode->containsNode(pNode.get()))
            m_pRootNode->addChild(pNode.get());

        m_pModelDataMap[sData.strName] = sData;
        m_pNodeMap[sData.strName] = pNode;
        // 设置材质
        _SetMaterial(pNode.get(), sData);
        return true;
    }
    return false;
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
        m_pMaterial->SetPBRMaterial(pNode);
    }
    break;
    case EGM_MATERIAL_Human:
    {
        m_pMaterial->SetHumanMaterial(pNode);
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

    // 设置阴影
    pStateSet->setDefine("SHADOW_RECEIVE", osg::StateAttribute::ON);

    return true;
}

bool CGMModel::SetAnimationEnable(const std::string& strName, const bool bEnable)
{
    osg::Node* pNode = _GetNode(strName);
    if (!pNode) return false;

    if(bEnable)
        return m_pAnimationManager->AddAnimation(strName, pNode);
    else
        return m_pAnimationManager->RemoveAnimation(strName);
}

bool CGMModel::GetAnimationEnable(const std::string& strName)
{
    osg::Node* pNode = _GetNode(strName);
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
