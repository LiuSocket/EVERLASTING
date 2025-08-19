//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2025~2045, LiuTao
/// All rights reserved.
///
/// @file		GMTerrain.cpp
/// @brief		GMEngine - Terrain Module
/// @version	1.0
/// @author		LiuTao
/// @date		2025.08.17
//////////////////////////////////////////////////////////////////////////
#include "GMTerrain.h"
#include "GMCommonUniform.h"
#include "GMLight.h"
#include "GMKit.h"

#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/MatrixTransform>
#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include <osg/CullFace>

using namespace GM;

/*************************************************************************
Global Constants
*************************************************************************/

/*************************************************************************
CGMTerrain Methods
*************************************************************************/
/** @brief 构造 */
CGMTerrain::CGMTerrain()
{
}

/** @brief 析构 */
CGMTerrain::~CGMTerrain()
{
}

/** @brief 初始化 */
bool CGMTerrain::Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData)
{
	m_pKernelData = pKernelData;
	m_pConfigData = pConfigData;

	m_pRootNode = new osg::Group;
	GM_Root->addChild(m_pRootNode.get());

	//地面不投射阴影但接收阴影
	//GM_LIGHT.SetCastShadowEnable(m_pRootNode, true);
	unsigned int iValue = osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE;
	osg::ref_ptr<osg::StateSet> pStateset = m_pRootNode->getOrCreateStateSet();
	pStateset->setTextureAttributeAndModes(SHADOW_TEX_UNIT, GM_LIGHT.GetShadowMap(), iValue);
	pStateset->addUniform(new osg::Uniform("texShadow", SHADOW_TEX_UNIT), iValue);
	pStateset->addUniform(GM_LIGHT.GetView2ShadowMatrixUniform());

	// 强制设置半透明混合模式
	pStateset->setMode(GL_BLEND, osg::StateAttribute::OFF);
	pStateset->setMode(GL_ALPHA_TEST, osg::StateAttribute::OFF);
	pStateset->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA),iValue);

	osg::ref_ptr<osg::Geode> pGeode = new osg::Geode();
	pGeode->addDrawable(_CreateTerrainGeometry());
	m_pRootNode->addChild(pGeode.get());

	m_pDDSOptions = new osgDB::Options("dds_flip");
	return true;
}

/** @brief 加载 */
bool CGMTerrain::Load()
{
	return true;
}

/** @brief 保存 */
bool CGMTerrain::Save()
{
	return true;
}

/** @brief 重置 */
bool CGMTerrain::Reset()
{
	return true;
}

bool CGMTerrain::Update(double dDeltaTime)
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

bool CGMTerrain::UpdatePost(double dDeltaTime)
{
	return true;
}

void CGMTerrain::ResizeScreen(const int width, const int height)
{
}

void CGMTerrain::_InnerUpdate(const double dDeltaTime)
{
}

osg::Geometry* CGMTerrain::_CreateTerrainGeometry() const
{

	return nullptr;
}

osg::Geometry* CGMTerrain::_MakeTileGeometry(const std::vector<int>& iTileVec, int iSegment) const
{
	return nullptr;
}
