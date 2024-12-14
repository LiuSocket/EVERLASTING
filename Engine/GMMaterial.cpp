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
#include "GMDispatchCompute.h"

#include <osg/TextureCubeMap>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>

#include <thread>
#include <ppl.h>

using namespace GM;

/*************************************************************************
Macro Defines
*************************************************************************/
#define PROBE_CUBESIZE					256		// probe的cubemap图片尺寸，单位：像素
#define PROBE_MIPMAP_NUM				5		// probe的mipmap最大层级数
#define PROBE_MIPMAP_BIN				10		// 生成probe第1级mipmap的renderbin
#define PROBE_COPY_BIN					20		// 将不同层的图片copy到cubemap的不同mipmap的renderbin

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
	class AutoAddTexVisitor : public osg::NodeVisitor
	{
	public:
		AutoAddTexVisitor(std::vector<osg::ref_ptr<osg::Texture2D>> vTex)
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

	/*
	*  @brief 人类模型访问器，用于给皮肤和眼睛添加材质
	*/
	class HumanVisitor : public osg::NodeVisitor
	{
	public:
		HumanVisitor(CGMMaterial* pMaterial)
			: osg::NodeVisitor(TRAVERSE_ALL_CHILDREN), _pMaterial(pMaterial){}

		void apply(osg::Node& node) { traverse(node); }

		void apply(osg::Geode& node)
		{
			if (node.getName().find("_skin") != std::string::npos)
			{
				// 如果节点名称中包含"_skin"，则设置皮肤材质
				_pMaterial->SetSSSMaterial(&node);
			}
			else if (node.getName().find("_eye") != std::string::npos)
			{
				// 根据节点名称找到眼睛模型，设置眼睛材质
				_pMaterial->SetEyeMaterial(&node);
			}
			else{}

			traverse(node);
		}

	private:

	// 成员变量
	private:
		CGMMaterial* _pMaterial = nullptr;
	};
}

/*************************************************************************
CGMMaterial Methods
*************************************************************************/

CGMMaterial::CGMMaterial():
	m_strModelShaderPath("Shaders/ModelShader/"), m_strDefTexPath("Textures/DefaultTexture/")
{
	m_iRandom.seed(0);
}

CGMMaterial::~CGMMaterial()
{
}

bool CGMMaterial::Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData, CGMCommonUniform* pCommonUniform)
{
	m_pKernelData = pKernelData;
	m_pConfigData = pConfigData;
	m_pCommonUniform = pCommonUniform;

	// 读取dds时需要垂直翻转
	m_pDDSOptions = new osgDB::Options("dds_flip");
	std::string strTexPath = m_pConfigData->strCorePath + m_strDefTexPath;

	// 临时功能
	//std::string strInputFilePath = strTexPath + "small_empty_room_3_8k.hdr";
	//std::string strOutputFilePath = strTexPath + "env_probe.dds";
	//_CreateProbe(strInputFilePath, strOutputFilePath);

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
	m_pEnvProbeTex->setImage(osgDB::readImageFile(strTexPath + "env_probe.dds"));
	m_pEnvProbeTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pEnvProbeTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);// 改成LINEAR_MIPMAP_LINEAR会报错
	m_pEnvProbeTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
	m_pEnvProbeTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);

	return true;
}

void CGMMaterial::SetPBRMaterial(osg::Node* pNode)
{
	//!< 自动补齐PBR模型的纹理单元
	AutoAddTexVisitor cAutoAddTexVisitor(m_pPBRTexVector);
	pNode->accept(cAutoAddTexVisitor);

	osg::ref_ptr<osg::StateSet> pStateSet = pNode->getOrCreateStateSet();
	// Uniform
	int iChannel = 0;
	_PlusUnitUsed(iChannel);
	// 漫反射贴图（RGB通道存放颜色，A通道存放不透明度）
	osg::ref_ptr<osg::Uniform> pTexBaseColorUniform = new osg::Uniform("texBaseColor", iChannel++);
	pStateSet->addUniform(pTexBaseColorUniform.get());
	_PlusUnitUsed(iChannel);
	// MRA贴图（R通道存放金属度，G通道存放粗糙度，B通道存放AO贴图，A通道待定）
	osg::ref_ptr<osg::Uniform> pTexPbrUniform = new osg::Uniform("texMRA", iChannel++);
	pStateSet->addUniform(pTexPbrUniform.get());
	_PlusUnitUsed(iChannel);
	// 自发光贴图（RGB通道存放颜色，A通道待定）
	osg::ref_ptr<osg::Uniform> pTexIlluminationUniform = new osg::Uniform("texIllumination", iChannel++);
	pStateSet->addUniform(pTexIlluminationUniform.get());
	_PlusUnitUsed(iChannel);
	// 法线贴图（RGB通道存放法线，A通道待定）
	osg::ref_ptr<osg::Uniform> pTexNormalUniform = new osg::Uniform("texNormal", iChannel++);
	pStateSet->addUniform(pTexNormalUniform.get());
	_PlusUnitUsed(iChannel);
	// 环境探针贴图
	CGMKit::AddTexture(pStateSet.get(), m_pEnvProbeTex.get(), "texEnvProbe", iChannel++);
	_PlusUnitUsed(iChannel);

	// 添加shader
	SetShader(pStateSet.get(), EGM_MATERIAL_PBR);
}

void CGMMaterial::SetHumanMaterial(osg::Node* pNode)
{
	HumanVisitor cHumanVisitor(this);
	pNode->accept(cHumanVisitor);
}

void CGMMaterial::SetSSSMaterial(osg::Node* pNode)
{
	osg::ref_ptr<osg::StateSet> pStateSet = pNode->getOrCreateStateSet();
	// Uniform
	int iChannel = 0;
	_PlusUnitUsed(iChannel);
	// 漫反射贴图（RGB通道存放颜色，A通道存放不透明度）
	osg::ref_ptr<osg::Uniform> pTexBaseColorUniform = new osg::Uniform("texBaseColor", iChannel++);
	pStateSet->addUniform(pTexBaseColorUniform.get());
	_PlusUnitUsed(iChannel);
	// MRA贴图（R通道存放金属度，G通道存放粗糙度，B通道存放AO贴图，A通道待定）
	osg::ref_ptr<osg::Uniform> pTexPbrUniform = new osg::Uniform("texMRA", iChannel++);
	pStateSet->addUniform(pTexPbrUniform.get());
	_PlusUnitUsed(iChannel);
	// 次表面贴图（RGB通道存放次表面颜色，A通道存放曲率图）
	osg::ref_ptr<osg::Uniform> pTexSSSRUniform = new osg::Uniform("texSSSR", iChannel++);
	pStateSet->addUniform(pTexSSSRUniform.get());
	_PlusUnitUsed(iChannel);
	// 法线贴图（RGB通道存放法线，A通道待定）
	osg::ref_ptr<osg::Uniform> pTexNormalUniform = new osg::Uniform("texNormal", iChannel++);
	pStateSet->addUniform(pTexNormalUniform.get());
	_PlusUnitUsed(iChannel);
	// 环境探针贴图
	CGMKit::AddTexture(pStateSet.get(), m_pEnvProbeTex.get(), "texEnvProbe", iChannel++);
	_PlusUnitUsed(iChannel);

	// 添加shader
	SetShader(pStateSet.get(), EGM_MATERIAL_SSS);
}

void CGMMaterial::SetEyeMaterial(osg::Node* pNode)
{
	osg::ref_ptr<osg::StateSet> pStateSet = pNode->getOrCreateStateSet();
	// Uniform
	int iChannel = 0;
	_PlusUnitUsed(iChannel);
	// 漫反射贴图（RGB通道存放虹膜和眼白的颜色，A通道待定）
	osg::ref_ptr<osg::Uniform> pTexBaseColorUniform = new osg::Uniform("texBaseColor", iChannel++);
	pStateSet->addUniform(pTexBaseColorUniform.get());
	_PlusUnitUsed(iChannel);
	// PR贴图（R通道存放“视差/Parallax”，G通道存放“粗糙度/Roughness”，B通道待定，A通道待定）
	osg::ref_ptr<osg::Uniform> pTexPRUniform = new osg::Uniform("texPR", iChannel++);
	pStateSet->addUniform(pTexPRUniform.get());
	_PlusUnitUsed(iChannel);
	// 法线贴图（RGB通道存放法线，A通道待定）
	osg::ref_ptr<osg::Uniform> pTexNormalUniform = new osg::Uniform("texNormal", iChannel++);
	pStateSet->addUniform(pTexNormalUniform.get());
	_PlusUnitUsed(iChannel);
	// 环境探针贴图
	CGMKit::AddTexture(pStateSet.get(), m_pEnvProbeTex.get(), "texEnvProbe", iChannel++);
	_PlusUnitUsed(iChannel);

	// 添加shader
	SetShader(pStateSet.get(), EGM_MATERIAL_Eye);
}

void CGMMaterial::SetBackgroundMaterial(osg::Node* pNode)
{
	osg::ref_ptr<osg::StateSet> pStateSet = pNode->getOrCreateStateSet();
	pStateSet->addUniform(m_pCommonUniform->GetScreenSize());

	// 添加shader
	SetShader(pStateSet.get(), EGM_MATERIAL_Background);
}

void CGMMaterial::SetShader(osg::StateSet* pSS, EGMMaterial eMaterial)
{
	if (!pSS) return;

	std::string strShaderPath = m_pConfigData->strCorePath + m_strModelShaderPath;
	switch (eMaterial)
	{
	case EGM_MATERIAL_PBR:
	{
		CGMKit::LoadShaderWithCommonFrag(pSS,
			strShaderPath + "GMPBR.vert",
			strShaderPath + "GMPBR.frag",
			strShaderPath + "GMCommon.frag",
			"PBR", true);
	}
	break;
	case EGM_MATERIAL_SSS:
	{
		CGMKit::LoadShaderWithCommonFrag(pSS,
			strShaderPath + "GMSSS.vert",
			strShaderPath + "GMSSS.frag",
			strShaderPath + "GMCommon.frag",
			"SSS", true);
	}
	break;
	case EGM_MATERIAL_Eye:
	{
		CGMKit::LoadShaderWithCommonFrag(pSS,
			strShaderPath + "GMEye.vert",
			strShaderPath + "GMEye.frag",
			strShaderPath + "GMCommon.frag",
			"Eye", true);
	}
	break;
	case EGM_MATERIAL_Background:
	{
		CGMKit::LoadShaderWithCommonFrag(pSS,
			strShaderPath + "GMBackground.vert",
			strShaderPath + "GMBackground.frag",
			strShaderPath + "GMCommon.frag",
			"Background");
	}
	break;
	default:
		break;
	}
}

bool CGMMaterial::_PlusUnitUsed(int& iUnit)
{
	if (SHADOW_TEX_UNIT == iUnit)
	{
		iUnit++;
		return false;
	}
	return true;
}
