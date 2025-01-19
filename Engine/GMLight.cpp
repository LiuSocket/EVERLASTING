//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMLight.cpp
/// @brief		GMEngine - Light Module
/// @version	1.0
/// @author		LiuTao
/// @date		2024.10.21
//////////////////////////////////////////////////////////////////////////
#include "GMLight.h"

#include <osg/Texture2D>
#include <osg/CullFace>

using namespace GM;

/*************************************************************************
Global Constants
*************************************************************************/

/*************************************************************************
CGMLight Methods
*************************************************************************/
/** @brief ���� */
CGMLight::CGMLight() :
    m_mView2ShadowUniform(new osg::Uniform("view2ShadowMatrix", osg::Matrixf()))
{
}

/** @brief ���� */
CGMLight::~CGMLight()
{
}

/** @brief ��ʼ�� */
bool CGMLight::Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData)
{
    m_pKernelData = pKernelData;
    m_pConfigData = pConfigData;

    // ������Դ
    osg::ref_ptr<osg::Light> pLight = new osg::Light(0);
    pLight->setPosition(osg::Vec4(m_vLightPos, 0));
    pLight->setAmbient(osg::Vec4(0.1, 0.12, 0.15, 1.0));
    pLight->setDiffuse(osg::Vec4(0.9, 0.88, 0.85, 1.0));
    pLight->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));

    m_pLightSource = new osg::LightSource;
    m_pLightSource->setLight(pLight.get());
    GM_Root->addChild(m_pLightSource);

    // ������Ӱ
    _InitShadow();

    return true;
}

/** @brief ���� */
bool CGMLight::Load()
{
    return true;
}

/** @brief ���� */
bool CGMLight::Save()
{
    return true;
}

/** @brief ���� */
bool CGMLight::Reset()
{
    return true;
}

/** @brief ���� */
bool CGMLight::Update(double dDeltaTime)
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

/** @brief ����(�����������֮��) */
bool CGMLight::UpdatePost(double dDeltaTime)
{
    osg::Matrixd biasMatrix(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );
    // set the texture coordinate generation matrix that the shadow
    // receiver will use to sample the shadow map. Doing this on the CPU
    // prevents nasty precision issues!
    osg::Matrixd inverseView = GM_View->getCamera()->getInverseViewMatrix();
    osg::Matrixd VPMatrix = m_pShadowCamera->getViewMatrix() * m_pShadowCamera->getProjectionMatrix();
    osg::Matrixf mView2ShadowMatrix = inverseView * VPMatrix * biasMatrix;
    m_mView2ShadowUniform->set(mView2ShadowMatrix);
    return true;
}

void CGMLight::SetShadowEnable(osg::Node* pNode)
{
    m_pShadowCamera->addChild(pNode);
}

void CGMLight::_InnerUpdate(const double dDeltaTime)
{
}

void CGMLight::_InitShadow()
{
    // ��ӰͶӰ�������
    float fHalfX = 20.0;
    float fHalfY = 20.0;
    float fNear = 0.0;
    float fFar = 500.0;
    // ��Ӱview�������
    osg::Vec3d vShadowPos(m_vLightPos);
    vShadowPos.normalize();
    vShadowPos *= 200.0;

    //��Ӱ��ͼ��С
    int iShadowMapSize = 1024;
    m_pShadowTexture = new osg::Texture2D;
    m_pShadowTexture->setTextureSize(iShadowMapSize, iShadowMapSize);
    m_pShadowTexture->setInternalFormat(GL_DEPTH_COMPONENT);
    m_pShadowTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
    m_pShadowTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
    m_pShadowTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
    m_pShadowTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
    m_pShadowTexture->setBorderColor(osg::Vec4d(1.0, 1.0, 1.0, 1.0));
    m_pShadowTexture->setDataVariance(osg::Object::DYNAMIC);
    m_pShadowTexture->setResizeNonPowerOfTwoHint(true);

    m_pShadowCamera = new osg::Camera();
    m_pShadowCamera->setName("shadowCamera");
    m_pShadowCamera->setClearDepth(1.0);
    m_pShadowCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
    m_pShadowCamera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    m_pShadowCamera->setCullMask(GM_SHADOW_CAST_MASK);
    m_pShadowCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF_INHERIT_VIEWPOINT);
    m_pShadowCamera->setRenderOrder(osg::Camera::PRE_RENDER);
    m_pShadowCamera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
    m_pShadowCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    m_pShadowCamera->setImplicitBufferAttachmentMask(
        osg::Camera::ImplicitBufferAttachment::IMPLICIT_DEPTH_BUFFER_ATTACHMENT,
        osg::Camera::ImplicitBufferAttachment::IMPLICIT_DEPTH_BUFFER_ATTACHMENT);
    m_pShadowCamera->attach(osg::Camera::DEPTH_BUFFER, m_pShadowTexture.get());
    m_pShadowCamera->setAllowEventFocus(false);
    m_pShadowCamera->setViewport(0, 0, iShadowMapSize, iShadowMapSize);
    m_pShadowCamera->setViewMatrixAsLookAt(vShadowPos, osg::Vec3d(0, 0, 0), osg::Vec3d(0, 0, 1));
    m_pShadowCamera->setProjectionMatrixAsOrtho(-fHalfX, fHalfX, -fHalfY, fHalfY, fNear, fFar);

    unsigned int iValue = osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE;
    osg::ref_ptr<osg::StateSet> pShadowSS = m_pShadowCamera->getOrCreateStateSet();
    pShadowSS->setDefine("SHADOW_CAST", iValue);
    pShadowSS->setMode(GL_CULL_FACE, iValue);
    pShadowSS->setAttributeAndModes(new osg::CullFace(osg::CullFace::FRONT), iValue);

    GM_Root->addChild(m_pShadowCamera.get());
}
