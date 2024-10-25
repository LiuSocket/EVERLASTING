//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2040, LT
/// All rights reserved.
///
/// @file		GMMaterial.cpp
/// @brief		GMEngine - Material manager
/// @version	1.0
/// @author		LiuTao
/// @date		2024.02.04
//////////////////////////////////////////////////////////////////////////

#include "GMMaterial.h"
#include "GMKit.h"
#include "GMCommon.h"

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>

using namespace GM;

/*************************************************************************
Global Constants
*************************************************************************/

/*************************************************************************
Class
*************************************************************************/

namespace GM
{
	/*
	* @brief 模型材质的drawable访问器，用于自动补齐末尾的未添加的纹理单元
	* 简化模型制作流程，这样模型可以只添加需要的贴图，但不能跳过某一个纹理单元添加后面的纹理单元
	* 正确纹理单元组合：0，0+1，0+1+2, 0+1+2+3，0+1+2+3+4
	* 错误纹理单元组合：0+2+3，2+4...
	*/
	class AutoTexVisitor : public osg::NodeVisitor
	{
	public:
		AutoTexVisitor(std::vector<osg::ref_ptr<osg::Texture2D>> vTex)
			: osg::NodeVisitor(TRAVERSE_ALL_CHILDREN), _vTex(vTex) {}

		void apply(osg::Node& node)
		{
			traverse(node);
		}
		void apply(osg::Geode& node)
		{
			for (unsigned int n = 0; n < node.getNumDrawables(); n++)
			{
				osg::Drawable* draw = node.getDrawable(n);
				if (draw)
				{
					osg::StateSet* pStateSet = draw->getOrCreateStateSet();
					// 获取所有需要的纹理单元
					std::vector<osg::StateAttribute*> vAttr;
					for (int i = 0; i < _vTex.size(); i++)
					{
						osg::StateAttribute* pAttr = pStateSet->getTextureAttribute(i, osg::StateAttribute::TEXTURE);
						vAttr.push_back(pAttr);
					}
					// 补齐空缺的纹理单元
					for (int i = 0; i < _vTex.size(); i++)
					{
						if (vAttr.at(i) || !_vTex.at(i).valid())
							continue;
						pStateSet->setTextureAttributeAndModes(i, _vTex.at(i).get(), osg::StateAttribute::ON);
					}

					draw->dirtyDisplayList();
				}
			}
			traverse(node);
		}

	private:
		std::vector<osg::ref_ptr<osg::Texture2D>> _vTex;
	};
}

/*************************************************************************
CGMMaterial Methods
*************************************************************************/

CGMMaterial::CGMMaterial(): m_pConfigData(nullptr), m_pCommonUniform(nullptr),
	m_strModelShaderPath("Shaders/ModelShader/"), m_strDefTexPath("Textures/DefaultTexture/")
{
}

CGMMaterial::~CGMMaterial()
{
}

bool CGMMaterial::Init(SGMConfigData* pConfigData, CGMCommonUniform* pCommonUniform)
{
	m_pConfigData = pConfigData;
	m_pCommonUniform = pCommonUniform;

	// 读取dds时需要垂直翻转
	m_pDDSOptions = new osgDB::Options("dds_flip");
	std::string strTexPath = m_pConfigData->strCorePath + m_strDefTexPath;
	// 初始化默认的各个材质的贴图，用于补齐纹理单元
	// 白色贴图
	osg::ref_ptr<osg::Texture2D> pWhiteTex = new osg::Texture2D;
	pWhiteTex->setImage(osgDB::readImageFile(strTexPath + "white.tga"));
	pWhiteTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture2D::NEAREST);
	pWhiteTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture2D::NEAREST);
	pWhiteTex->setWrap(osg::Texture::WRAP_S, osg::Texture2D::REPEAT);
	pWhiteTex->setWrap(osg::Texture::WRAP_T, osg::Texture2D::REPEAT);
	pWhiteTex->setInternalFormat(GL_RGBA8);
	pWhiteTex->setSourceFormat(GL_RGBA);
	pWhiteTex->setSourceType(GL_UNSIGNED_BYTE);
	// 黑色贴图
	osg::ref_ptr<osg::Texture2D> pBlackTex = new osg::Texture2D;
	pBlackTex->setImage(osgDB::readImageFile(strTexPath + "black.tga"));
	pBlackTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture2D::NEAREST);
	pBlackTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture2D::NEAREST);
	pBlackTex->setWrap(osg::Texture::WRAP_S, osg::Texture2D::REPEAT);
	pBlackTex->setWrap(osg::Texture::WRAP_T, osg::Texture2D::REPEAT);
	pBlackTex->setInternalFormat(GL_RGBA8);
	pBlackTex->setSourceFormat(GL_RGBA);
	pBlackTex->setSourceType(GL_UNSIGNED_BYTE);
	// 默认MRA贴图
	osg::ref_ptr<osg::Texture2D> pMRATex = new osg::Texture2D;
	pMRATex->setImage(osgDB::readImageFile(strTexPath + "default_MRA.tga"));
	pMRATex->setFilter(osg::Texture::MIN_FILTER, osg::Texture2D::NEAREST);
	pMRATex->setFilter(osg::Texture::MAG_FILTER, osg::Texture2D::NEAREST);
	pMRATex->setWrap(osg::Texture::WRAP_S, osg::Texture2D::REPEAT);
	pMRATex->setWrap(osg::Texture::WRAP_T, osg::Texture2D::REPEAT);
	pMRATex->setInternalFormat(GL_RGBA8);
	pMRATex->setSourceFormat(GL_RGBA);
	pMRATex->setSourceType(GL_UNSIGNED_BYTE);
	// 默认法线贴图
	osg::ref_ptr<osg::Texture2D> pNormalTex = new osg::Texture2D;
	pNormalTex->setImage(osgDB::readImageFile(strTexPath + "default_n.tga"));
	pNormalTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture2D::NEAREST);
	pNormalTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture2D::NEAREST);
	pNormalTex->setWrap(osg::Texture::WRAP_S, osg::Texture2D::REPEAT);
	pNormalTex->setWrap(osg::Texture::WRAP_T, osg::Texture2D::REPEAT);
	pNormalTex->setInternalFormat(GL_RGBA8);
	pNormalTex->setSourceFormat(GL_RGBA);
	pNormalTex->setSourceType(GL_UNSIGNED_BYTE);
	
	//!< PBR模型的纹理单元默认贴图
	m_pPBRTexVector.push_back(pWhiteTex); // 0 基础颜色
	m_pPBRTexVector.push_back(pMRATex); // 1 金属度、粗糙度、AO
	m_pPBRTexVector.push_back(pBlackTex); // 2 自发光贴图
	m_pPBRTexVector.push_back(pNormalTex); // 3 法线贴图

	// 初始化所有公用图片资源
	// 水面涟漪
	m_pRainRippleTex = new osg::Texture2D;
	m_pRainRippleTex->setImage(osgDB::readImageFile(strTexPath + "ripple.tga"));
	m_pRainRippleTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pRainRippleTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture2D::LINEAR);
	m_pRainRippleTex->setWrap(osg::Texture::WRAP_S, osg::Texture2D::REPEAT);
	m_pRainRippleTex->setWrap(osg::Texture::WRAP_T, osg::Texture2D::REPEAT);
	m_pRainRippleTex->setInternalFormat(GL_RGBA8);
	m_pRainRippleTex->setSourceFormat(GL_RGBA);
	m_pRainRippleTex->setSourceType(GL_UNSIGNED_BYTE);
	// 潮湿表面法线贴图
	m_pWetNormalTex = new osg::Texture2D;
	m_pWetNormalTex->setImage(osgDB::readImageFile(strTexPath + "custom_n.dds", m_pDDSOptions));
	m_pWetNormalTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pWetNormalTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
	m_pWetNormalTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
	m_pWetNormalTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
	// 噪声贴图
	m_pNoiseTex = new osg::Texture2D;
	m_pNoiseTex->setImage(osgDB::readImageFile(strTexPath + "custom_noise.dds", m_pDDSOptions));
	m_pNoiseTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pNoiseTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
	m_pNoiseTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
	m_pNoiseTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
	// 雪和霜的贴图
	m_pSnowTex = new osg::Texture2D;
	m_pSnowTex->setImage(osgDB::readImageFile(strTexPath + "snow.dds", m_pDDSOptions));
	m_pSnowTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pSnowTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
	m_pSnowTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
	m_pSnowTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
	// 黄沙贴图
	m_pSandTex = new osg::Texture2D;
	m_pSandTex->setImage(osgDB::readImageFile(strTexPath + "sand.dds", m_pDDSOptions));
	m_pSandTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pSandTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
	m_pSandTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
	m_pSandTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
	// 环境探针贴图
	m_pEnvProbeTex = new osg::Texture2D;
	m_pEnvProbeTex->setImage(osgDB::readImageFile(strTexPath + "env_probe.dds", m_pDDSOptions));
	m_pEnvProbeTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pEnvProbeTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);// 改成LINEAR_MIPMAP_LINEAR会报错
	m_pEnvProbeTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
	m_pEnvProbeTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);

	return true;
}

void CGMMaterial::SetPBRShader(osg::Node* pNode)
{
	//!< 自动补齐地面模型的纹理单元
	AutoTexVisitor cAutoTexVisitor(m_pPBRTexVector);
	pNode->accept(cAutoTexVisitor);

	osg::StateSet* pStateSet = new osg::StateSet();
	// 添加shader
	std::string strShaderPath = m_pConfigData->strCorePath + m_strModelShaderPath;
	CGMKit::LoadShaderWithCommonFrag(pStateSet,
		strShaderPath + "GMModel.vert",
		strShaderPath + "GMModel.frag",
		strShaderPath + "GMCommon.frag",
		"Model");

	// Uniform
	int iChannel = 0;
	// 漫反射贴图（RGB通道存放颜色，A通道存放不透明度）
	osg::ref_ptr<osg::Uniform> pTexBaseColorUniform = new osg::Uniform("texBaseColor", iChannel++);//纹理单元不要乱分配！
	pStateSet->addUniform(pTexBaseColorUniform.get());

	// MRA贴图（R通道存放金属度，G通道存放粗糙度，B通道存放AO贴图，A通道待定）
	osg::ref_ptr<osg::Uniform> pTexPbrUniform = new osg::Uniform("texMRA", iChannel++);
	pStateSet->addUniform(pTexPbrUniform.get());
	// 自发光贴图（RGB通道存放颜色，A通道待定）
	osg::ref_ptr<osg::Uniform> pTexIlluminationUniform = new osg::Uniform("texIllumination", iChannel++);
	pStateSet->addUniform(pTexIlluminationUniform.get());
	// 法线贴图（RGB通道存放法线，A通道待定）
	osg::ref_ptr<osg::Uniform> pTexNormalUniform = new osg::Uniform("texNormal", iChannel++);
	pStateSet->addUniform(pTexNormalUniform.get());
	// 环境探针贴图
	CGMKit::AddTexture(pStateSet, m_pEnvProbeTex.get(), "texEnvProbe", iChannel++);

	pNode->setStateSet(pStateSet);
}

void CGMMaterial::SetBackgroundShader(osg::Node* pNode)
{
	osg::StateSet* pStateSet = new osg::StateSet();

	pStateSet->addUniform(m_pCommonUniform->GetScreenSize());

	// 添加shader
	std::string strShaderPath = m_pConfigData->strCorePath + m_strModelShaderPath;
	CGMKit::LoadShader(pStateSet,
		strShaderPath + "GMBackground.vert",
		strShaderPath + "GMBackground.frag",
		"Background");

	pNode->setStateSet(pStateSet);
}
