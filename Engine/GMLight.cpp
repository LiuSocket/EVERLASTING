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
#include <osg/BufferObject>

using namespace GM;

/*************************************************************************
Global Constants
*************************************************************************/

/*************************************************************************
CGMLight Methods
*************************************************************************/

template<> CGMLight* CGMSingleton<CGMLight>::msSingleton = nullptr;

/** @brief 获取单例 */
CGMLight& CGMLight::getSingleton(void)
{
	if (!msSingleton)
		msSingleton = GM_NEW(CGMLight);
	assert(msSingleton);
	return (*msSingleton);
}

/** @brief 构造 */
CGMLight::CGMLight() :
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

	osg::ref_ptr<osg::UniformBufferObject> pLightDataUBO = new osg::UniformBufferObject;
	pLightDataUBO->setUsage(GL_DYNAMIC_DRAW);
	m_pLightBuffer = new osg::BufferTemplate<SLightDataBuffer>();
	m_pLightBuffer->setBufferObject(pLightDataUBO.get());
	m_pLightUBB = new osg::UniformBufferBinding(0, m_pLightBuffer.get(), 0, m_pLightBuffer->getTotalDataSize());
	m_pLightUBB->setDataVariance(osg::Object::DYNAMIC);

	// 创建阴影
	_InitShadow();

	return true;
}

void CGMLight::Release()
{
	// 清空光源
	Clear();

	GM_DELETE(msSingleton);
}

bool CGMLight::Update(double dDeltaTime)
{
	return true;
}

/** @brief 更新(在主相机更新之后) */
bool CGMLight::UpdatePost(double dDeltaTime)
{
	osg::Matrixd mainView = GM_View->getCamera()->getViewMatrix();
	// 多光源更新
	int iID = 0;
	for (auto& itr : m_mapLight)
	{
		osg::Vec4d vViewPos = itr.second.vPos * mainView;
		osg::Vec4d vViewDir = itr.second.vDir * mainView;
		osg::Vec3f vColor = itr.second.vColor * itr.second.fLuminous * 2e-4;
		float fAngle = itr.second.fAngle;
		float fSpotExponent = itr.second.fSpotExponent;
		float fRange = 0.1f * fmax(1.0f, itr.second.fLuminous);
		m_pLightBuffer->getData().SetLight(
			osg::Vec3f(vViewPos.x(), vViewPos.y(), vViewPos.z()),
			osg::Vec3f(vViewDir.x(), vViewDir.y(), vViewDir.z()),
			vColor, fAngle, fSpotExponent, fRange,
			iID);
		iID++;
	}
	if (0 != iID) m_pLightBuffer->dirty();

	// 更新阴影
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

bool CGMLight::Add(SLightData& sData)
{
	if ("" == sData.strName) return false;
	// 创建光源
	if (m_mapLight.find(sData.strName) == m_mapLight.end())
	{
		sData.vDir.normalize();
		m_mapLight[sData.strName] = sData;

		if (EGMLIGHT_SOURCE_DIRECTIONAL == sData.eType && sData.bShadow)
		{
			osg::Vec3d vShadowPos = -osg::Vec3d(sData.vDir.x(), sData.vDir.y(), sData.vDir.z())*200;
			m_pShadowCamera->setViewMatrixAsLookAt(vShadowPos, osg::Vec3d(0, 0, 0), osg::Vec3d(0, 0, 1));
		}

		if (m_pLightBuffer->getData()._lightNum.x() < GM_MAX_LIGHTNUM)
			m_pLightBuffer->getData()._lightNum.x()++;

		return true;
	}
	return false;
}

bool CGMLight::Edit(const SLightData& sData)
{
	if ("" == sData.strName) return false;
	// 修改光源
	if (m_mapLight.find(sData.strName) != m_mapLight.end())
	{
		m_mapLight.at(sData.strName) = sData;
		return true;
	}
	return false;
}

bool CGMLight::Remove(const std::string& strName)
{
	if ("" == strName) return false;
	// 移除光源
	if (m_mapLight.find(strName) != m_mapLight.end())
	{
		m_mapLight.erase(strName);

		if (m_mapLight.size() < m_pLightBuffer->getData()._lightNum.x())
			m_pLightBuffer->getData()._lightNum.x()--;

		if (m_pLightBuffer->getData()._lightNum.x() < 0)
			m_pLightBuffer->getData()._lightNum.x() = 0;
		return true;
	}
	return false;
}

bool CGMLight::Clear()
{
	// 清空光源
	m_mapLight.clear();
	m_pLightBuffer->getData()._lightNum = osg::Vec4f(.0f, .0f, .0f, 0.0f);
	return true;
}

bool CGMLight::Find(SLightData& sData)
{
	// 查找光源
	if (m_mapLight.find(sData.strName) != m_mapLight.end())
	{
		sData = m_mapLight[sData.strName];
		return true;
	}
	return false;
}

bool CGMLight::SetLightEnable(osg::Node* pNode, bool bEnable)
{
	osg::StateSet* pSS = pNode->getStateSet();
	if (!pSS) return false;
	osg::StateAttribute* pSA = pSS->getAttribute(osg::StateAttribute::PROGRAM);
	if (!pSA) return false;
	osg::Program* pProgram = dynamic_cast<osg::Program*>(pSA);
	if (!pProgram) return false;

	if (bEnable)
	{
		pSS->setDefine("GM_MAX_LIGHTNUM", std::to_string(int(GM_MAX_LIGHTNUM)), osg::StateAttribute::ON);
		pSS->setAttributeAndModes(m_pLightUBB, osg::StateAttribute::ON);
		pProgram->addBindUniformBlock("LightDataBlock", 0);
	}
	else
	{
		pSS->removeDefine("GM_MAX_LIGHTNUM");
		pSS->removeAttribute(m_pLightUBB);
		// 由于共用program，暂时不能解除UBO绑定
		//pProgram->removeBindUniformBlock("LightDataBlock");
	}

	return true;
}

void CGMLight::SetCastShadowEnable(osg::Node* pNode, bool bEnable)
{
	if (!pNode) return;

	if (bEnable)
	{
		if(!m_pShadowCamera->containsNode(pNode))
			m_pShadowCamera->addChild(pNode);
	}
	else
	{
		m_pShadowCamera->removeChild(pNode);
	}
}

void CGMLight::_InitShadow()
{
	// 阴影投影矩阵参数
	float fHalfX = 20.0;
	float fHalfY = 20.0;
	float fNear = 0.0;
	float fFar = 500.0;

	//!< 默认阴影位置
	osg::Vec3d vShadowPos = osg::Vec3d(1, -2, 1.5);
	vShadowPos.normalize();
	vShadowPos *= 200.0;

	//阴影贴图大小
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
	m_pShadowCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
	m_pShadowCamera->setCullMask(GM_SHADOW_CAST_MASK);
	m_pShadowCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
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