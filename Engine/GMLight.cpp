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

using namespace GM;

/*************************************************************************
Global Constants
*************************************************************************/

/*************************************************************************
CGMLight Methods
*************************************************************************/
/** @brief 构造 */
CGMLight::CGMLight() :
	m_pKernelData(nullptr), m_pConfigData(nullptr)
{
}

/** @brief 析构 */
CGMLight::~CGMLight()
{
}

/** @brief 初始化 */
bool CGMLight::Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData)
{
	m_pKernelData = pKernelData;
	m_pConfigData = pConfigData;

	osg::ref_ptr<osg::Light> pLight = new osg::Light(0);
	pLight->setPosition(osg::Vec4(1, -2, 1.5, 0));
	pLight->setAmbient(osg::Vec4(0.1, 0.12, 0.15, 1.0));
	pLight->setDiffuse(osg::Vec4(0.9, 0.88, 0.85, 1.0));
	pLight->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));

	m_pLightSource = new osg::LightSource;
	m_pLightSource->setLight(pLight.get());
	GM_Root->addChild(m_pLightSource);

	//创建阴影
	int iShadowMapSize = 1024;
	int iHalfSize = iShadowMapSize / 2;

	m_pShadowTexture = new osg::Texture2D();
	m_pShadowTexture->setTextureSize(iShadowMapSize, iShadowMapSize);
	m_pShadowTexture->setInternalFormat(GL_R32F);
	m_pShadowTexture->setSourceFormat(GL_RED);
	m_pShadowTexture->setSourceType(GL_FLOAT);
	m_pShadowTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	m_pShadowTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	m_pShadowTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
	m_pShadowTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
	m_pShadowTexture->setBorderColor(osg::Vec4d(0, 0, 0, 0));
	m_pShadowTexture->setDataVariance(osg::Object::DYNAMIC);
	m_pShadowTexture->setResizeNonPowerOfTwoHint(true);

	m_pShadowCamera = new osg::Camera();
	m_pShadowCamera->setName("shadowCamera");
	m_pShadowCamera->setClearColor(osg::Vec4(0.0, 0.0, 0.0, 0.0));
	m_pShadowCamera->setCullMask(GM_SHADOW_CAST_MASK);
	m_pShadowCamera->setRenderOrder(osg::Camera::PRE_RENDER);
	m_pShadowCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_pShadowCamera->setViewport(new osg::Viewport(0, 0, iShadowMapSize, iShadowMapSize));
	m_pShadowCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	m_pShadowCamera->setAllowEventFocus(false);
	m_pShadowCamera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	m_pShadowCamera->setViewMatrixAsLookAt(osg::Vec3(1, -2, 1.5), osg::Vec3(0, 0, 0), osg::Vec3(0, 0, 1));
	m_pShadowCamera->setProjectionMatrixAsOrtho(-iHalfSize, iHalfSize, -iHalfSize, iHalfSize, -5, 5);

	GM_Root->addChild(m_pShadowCamera.get());

	return true;
}

/** @brief 加载 */
bool CGMLight::Load()
{
	return true;
}

/** @brief 保存 */
bool CGMLight::Save()
{
	return true;
}

/** @brief 重置 */
bool CGMLight::Reset()
{
	return true;
}

/** @brief 更新 */
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

/** @brief 更新(在主相机更新之后) */
bool CGMLight::UpdatePost(double dDeltaTime)
{
	return true;
}

void CGMLight::AddShadowNode(osg::Node* pNode)
{
	m_pShadowCamera->addChild(pNode);
}

void CGMLight::_InnerUpdate(const double dDeltaTime)
{
}