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
	m_pDDSOptions = new osgDB::Options("dds_flip");
	m_pRootNode = new osg::Group;
	//GM_Root->addChild(m_pRootNode.get());

	osg::ref_ptr<osg::UniformBufferObject> pTerrDataUBO = new osg::UniformBufferObject;
	pTerrDataUBO->setUsage(GL_DYNAMIC_DRAW);
	m_pTerrSRTBuffer = new osg::BufferTemplate<STerrainBuffer>();
	m_pTerrSRTBuffer->setBufferObject(pTerrDataUBO.get());
	m_pTerrSRTUBB = new osg::UniformBufferBinding(0, m_pTerrSRTBuffer.get(), 0, m_pTerrSRTBuffer->getTotalDataSize());
	m_pTerrSRTUBB->setDataVariance(osg::Object::DYNAMIC);

	//地面不投射阴影但接收阴影
	//GM_LIGHT.SetCastShadowEnable(m_pRootNode, true);
	unsigned int iValue = osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE;
	osg::ref_ptr<osg::StateSet> pStateSet = m_pRootNode->getOrCreateStateSet();
	pStateSet->setTextureAttributeAndModes(SHADOW_TEX_UNIT, GM_LIGHT.GetShadowMap(), iValue);
	pStateSet->addUniform(new osg::Uniform("texShadow", SHADOW_TEX_UNIT), iValue);
	pStateSet->addUniform(GM_LIGHT.GetView2ShadowMatrixUniform());

	// 强制设置半透明混合模式
	pStateSet->setMode(GL_BLEND, osg::StateAttribute::OFF);
	pStateSet->setAttributeAndModes(new osg::CullFace(), osg::StateAttribute::ON);
	pStateSet->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA),iValue);

	m_pTerrainTrans = new osg::MatrixTransform();
	m_pTerrainGeode = new osg::Geode();
	m_pTerrainGeode->addDrawable(_MakeTerrainBlockGeometry(BLOCK_SEGMENT));
	m_pTerrainTrans->addChild(m_pTerrainGeode.get());
	m_pRootNode->addChild(m_pTerrainTrans.get());

	std::string strShaderPath = m_pConfigData->strCorePath + m_strTerrainShaderPath;
	osg::ref_ptr<osg::StateSet> pSS = m_pTerrainGeode->getOrCreateStateSet();
	pSS->setDefine("TERRAIN_RECT_MAX", std::to_string(int(TERRAIN_RECT_MAX)), osg::StateAttribute::ON);

	CGMKit::LoadShader(pSS, strShaderPath + "TerrainBlock.vert", strShaderPath + "TerrainBlock.frag");

	osg::Program* pProgram = CGMKit::GetProgram(CGMKit::GetProgramName(
		strShaderPath + "TerrainBlock.vert", strShaderPath + "TerrainBlock.frag"));
	pSS->setAttributeAndModes(m_pTerrSRTUBB, osg::StateAttribute::ON);
	if(pProgram) pProgram->addBindUniformBlock("TerrainBlock", 0);

	return true;
}

/** @brief 加载 */
bool CGMTerrain::Load()
{
	std::string strShaderPath = m_pConfigData->strCorePath + m_strTerrainShaderPath;
	CGMKit::LoadShader(m_pTerrainGeode->getStateSet(),
		strShaderPath + "TerrainBlock.vert",
		strShaderPath + "TerrainBlock.frag",
		true);
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
	osg::Vec3d vEye, vCenter, vUp;
	GM_View->getCamera()->getViewMatrixAsLookAt(vEye, vCenter, vUp);
	double fovy, aspectRatio, zNear, zFar;
	GM_View->getCamera()->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

	osg::Matrixd terrTransMat = osg::Matrixd::identity();
	terrTransMat.setTrans(osg::Vec3d(vEye.x(), vEye.y(),0));
	m_pTerrainTrans->setMatrix(terrTransMat);

	// 获取相机下方瓦片的平均高程，再根据相机与之的距离和视角，修改中心圈层尺寸
	double fCenterMeanElevation = 0.0;
	double fEyeAlt = vEye.z();
	m_fMinSegSize = (fEyeAlt - fCenterMeanElevation) / BLOCK_SEGMENT;
	//必须保证是2的幂
	m_fMinSegSize = max(1.0, std::exp2(int(1 + std::log2(max(0.1, m_fMinSegSize)))));

	// 根据视锥参数判断中心瓦片需要留多少积木
	// 按照笛卡尔坐标系四象限的顺序，逆时针旋转
	//					Y轴
	//	方块2（西北）	|	方块1（东北）
	//	第二象限		|	第一象限
	//------------------0----------------- X轴
	//	方块3（西南）	|	方块4（东南）
	//	第三象限		|	第四象限
	//					|
	float fMinBlockSize = m_fMinSegSize * BLOCK_SEGMENT;
	osg::Vec2f vPos00 = osg::Vec2f(1, 1) * 0.5 * fMinBlockSize;
	osg::Vec2f vPos01 = osg::Vec2f(-1, 1) * 0.5 * fMinBlockSize;
	m_pTerrSRTBuffer->getData().SetRectanglePosAndScale(0, m_fMinSegSize, vPos00);
	m_pTerrSRTBuffer->getData().SetRectanglePosAndScale(1, m_fMinSegSize, vPos01);
	//m_pTerrSRTBuffer->getData().SetRectanglePosAndScale(2, 0);
	//m_pTerrSRTBuffer->getData().SetRectanglePosAndScale(3, 0);

	float fRingRadius = 1.5;
	for (int iRing = 1; iRing <= TERRAIN_RING_NUM; iRing++)
	{
		// 根据视锥参数判断周围环状瓦片的每一层需要留多少个方块积木
		//-----------------------------------------------------------
		//	5 4 3 2	|	4 3 2 1	|	3 2 1 *
		//	      1	|	5		|	4
		//			|			|	5
		//-----------------------------------------------------------
		//	* 4 3 2	|	4 3 2 1	|	3 2 1 *
		//        1	|			|	4
		//-----------------------------------------------------------
		//	* * 3 2	|	* 3 2 1 |	3 2 1 *
		//	      1	|			|
		//-----------------------------------------------------------
		//	      2	|	* * 2 1	|	* 2 1 *	|	2 1 * *
		//	      1	|
		//-----------------------------------------------------------

		float fRingCount = std::fmaxf(0, iRing - 1);
		// 根据所在圈层，计算缩放比例
		float fRingScale = std::exp2(fRingCount);
		// 根据所在圈层，计算缩放比例
		float fSegSize = m_fMinSegSize * fRingScale;

		//int iStartID = 4 + 5 * (iRing - 1);
		int iStartID = 2 + 2 * (iRing - 1);
		osg::Vec2f vPos0 = osg::Vec2f(0.5 * fRingScale, fRingRadius) * fMinBlockSize;
		osg::Vec2f vPos1 = osg::Vec2f(-0.5 * fRingScale, fRingRadius) * fMinBlockSize;
		m_pTerrSRTBuffer->getData().SetRectanglePosAndScale(iStartID, fSegSize, vPos0);
		m_pTerrSRTBuffer->getData().SetRectanglePosAndScale(iStartID + 1, fSegSize, vPos1);
		//m_pTerrSRTBuffer->getData().SetRectanglePosAndScale(iStartID + 2, 0);
		//m_pTerrSRTBuffer->getData().SetRectanglePosAndScale(iStartID + 3, 0);
		//m_pTerrSRTBuffer->getData().SetRectanglePosAndScale(iStartID + 4, 0);

		fRingRadius += fRingScale * 1.5; // to do
	}

	return true;
}

void CGMTerrain::ResizeScreen(const int width, const int height)
{
}

void CGMTerrain::_InnerUpdate(const double dDeltaTime)
{
}

osg::Geometry* CGMTerrain::_MakeTerrainBlockGeometry(int iSegment) const
{
	// 为了效率，限制iSegment的上限，以防总快数超过65536，特意设置成2^n-1是为了保证高程图的分辨率是2^n
	iSegment = osg::clampBetween(iSegment, 3, 255);
	double fHalfSize = iSegment*0.5;
	int iVertPerEdge = iSegment + 1;
	int iVertPerFace = iVertPerEdge * iVertPerEdge;
	int iEleCount = iSegment * iSegment * 6;

	osg::Geometry* geom = new osg::Geometry();
	geom->setUseDisplayList(false);
	geom->setUseVertexBufferObjects(true);

	osg::Vec3Array* verts = new osg::Vec3Array();
	verts->reserve(iVertPerFace);
	GLuint* pEle = new GLuint[iEleCount];
	for (int i = 0; i < iVertPerEdge; i++)
	{
		for (int j = 0; j < iVertPerEdge; j++)
		{
			verts->push_back(osg::Vec3( i - fHalfSize, j - fHalfSize, 0.0));
			if (i < iSegment && j < iSegment)
			{
				int index = (i * iSegment + j) * 6;
				pEle[index] = i * iVertPerEdge + j;
				pEle[index + 1] = (i + 1) * iVertPerEdge + j;
				pEle[index + 2] = (i + 1) * iVertPerEdge + (j + 1);
				pEle[index + 3] = i * iVertPerEdge + j;
				pEle[index + 4] = (i + 1) * iVertPerEdge + (j + 1);
				pEle[index + 5] = i * iVertPerEdge + (j + 1);
			}
		}
	}
	osg::DrawElementsUInt* el = new osg::DrawElementsUInt(GL_TRIANGLES, iEleCount, pEle, TERRAIN_RECT_MAX);
	geom->setVertexArray(verts);
	geom->addPrimitiveSet(el);

	const float fBlockSize = 1000.0f;//10米
	geom->setInitialBound(osg::BoundingBox(-fBlockSize, -fBlockSize, -fBlockSize, fBlockSize, fBlockSize, fBlockSize));
	return geom;
}
