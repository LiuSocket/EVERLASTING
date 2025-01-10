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
/** @brief 构造 */
CGMLight::CGMLight() :
	m_pKernelData(nullptr), m_pConfigData(nullptr),
	m_mView2ShadowUniform(new osg::Uniform("view2ShadowMatrix", osg::Matrixf()))
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

	// 创建光源
	osg::Vec3d vLightPos(1, -2, 1.5);
	osg::ref_ptr<osg::Light> pLight = new osg::Light(0);
	pLight->setPosition(osg::Vec4(vLightPos, 0));
	pLight->setAmbient(osg::Vec4(0.1, 0.12, 0.15, 1.0));
	pLight->setDiffuse(osg::Vec4(0.9, 0.88, 0.85, 1.0));
	pLight->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));

	m_pLightSource = new osg::LightSource;
	m_pLightSource->setLight(pLight.get());
	GM_Root->addChild(m_pLightSource);

	// 创建阴影

	// 阴影投影矩阵参数
	float fHalfX = 50.0;
	float fHalfY = 50.0;
	float fNear = 0.0;
	float fFar = 100.0;
	// 阴影view矩阵参数
	osg::Vec3d vShadowPos(vLightPos);
	vShadowPos.normalize();
	vShadowPos *= 50.0;

	//阴影贴图大小
	int iShadowMapSize = 1024;
	m_pShadowTexture = new osg::Texture2D;
	m_pShadowTexture->setTextureSize(iShadowMapSize, iShadowMapSize);
	m_pShadowTexture->setInternalFormat(GL_DEPTH_COMPONENT);
	m_pShadowTexture->setSourceFormat(GL_DEPTH_COMPONENT);
	m_pShadowTexture->setSourceType(GL_FLOAT);
	m_pShadowTexture->setShadowComparison(true);
	m_pShadowTexture->setShadowTextureMode(osg::Texture2D::LUMINANCE);
	m_pShadowTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
	m_pShadowTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
	m_pShadowTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
	m_pShadowTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
	m_pShadowTexture->setBorderColor(osg::Vec4d(1.0, 1.0, 1.0, 1.0));

	m_pShadowCamera = new osg::Camera();
	m_pShadowCamera->setName("shadowCamera");
	m_pShadowCamera->setClearColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	m_pShadowCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
	m_pShadowCamera->setCullMask(GM_SHADOW_CAST_MASK);
	m_pShadowCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	m_pShadowCamera->setRenderOrder(osg::Camera::PRE_RENDER);
	m_pShadowCamera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	m_pShadowCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	m_pShadowCamera->attach(osg::Camera::DEPTH_BUFFER, m_pShadowTexture.get());
	m_pShadowCamera->setAllowEventFocus(false);
	m_pShadowCamera->setViewport(new osg::Viewport(0, 0, iShadowMapSize, iShadowMapSize));
	m_pShadowCamera->setViewMatrixAsLookAt(vShadowPos, osg::Vec3d(0, 0, 0), osg::Vec3d(0, 0, 1));
	m_pShadowCamera->setProjectionMatrixAsOrtho(-fHalfX, fHalfX, -fHalfY, fHalfY, fNear, fFar);

	unsigned int iValue = osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE;
	osg::ref_ptr<osg::StateSet> pShadowSS = m_pShadowCamera->getOrCreateStateSet();
	pShadowSS->setDefine("SHADOW_CAST", iValue);
	pShadowSS->setAttributeAndModes(new osg::CullFace(osg::CullFace::FRONT), iValue);
	pShadowSS->setMode(GL_CULL_FACE, iValue);

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
	osg::Matrixd biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);
	// set the texture coordinate generation matrix that the shadow
	// receiver will use to sample the shadow map. Doing this on the CPU
	// prevents nasty precision issues!
	osg::Matrixd inverseView = GM_Viewer->getCamera()->getInverseViewMatrix();
	osg::Matrixd VPMatrix = m_pShadowCamera->getViewMatrix() * m_pShadowCamera->getProjectionMatrix();
	osg::Matrixf mView2ShadowMatrix = inverseView * VPMatrix * biasMatrix;

	m_mView2ShadowUniform->set(mView2ShadowMatrix);
	return true;
}

void CGMLight::AddShadowNode(osg::Node* pNode)
{
	m_pShadowCamera->addChild(pNode);
}

void CGMLight::_InnerUpdate(const double dDeltaTime)
{
}