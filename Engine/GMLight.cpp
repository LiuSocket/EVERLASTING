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

using namespace GM;

/*************************************************************************
Global Constants
*************************************************************************/

namespace GM
{
}

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

void CGMLight::_InnerUpdate(const double dDeltaTime)
{
}