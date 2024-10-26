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
	class CopyMipmapCallback : public osg::Drawable::DrawCallback
	{
	public:
		CopyMipmapCallback(std::vector<osg::ref_ptr<osg::TextureCubeMap>>& src, osg::TextureCubeMap* dst)
			: _pSrcTexVec(src), _pDstTex(dst)
		{
			_fbo = new osg::FrameBufferObject();
		}

		void drawImplementation(osg::RenderInfo& renderInfo, const osg::Drawable* drawable) const
		{
			osg::State* state = renderInfo.getState();
			const unsigned int iContextID = renderInfo.getContextID();

			// get the texture object for the current contextID.
			osg::Texture::TextureObject* textureObject = _pDstTex->getTextureObject(iContextID);
			if (!textureObject)
			{
				// create texture object.
				_pDstTex->apply(*state);

				textureObject = _pDstTex->getTextureObject(iContextID);

				if (!textureObject)
				{
					// failed to create texture object
					OSG_NOTICE << "Warning : failed to create TextureCubeMap texture obeject, copyTexSubImageCubeMap abandoned." << std::endl;
					return;
				}
			}
			if (textureObject)
			{
				// we have a valid image
				textureObject->bind();

				int iLevelMax = min(int(log2(_pDstTex->getTextureWidth())), PROBE_MIPMAP_NUM);
				for (int iLevel = 0; iLevel <= iLevelMax; iLevel++)
				{
					for (unsigned int iFace = 0; iFace < 6; iFace++)
					{
						_fbo->setAttachment(
							osg::Camera::COLOR_BUFFER,
							osg::FrameBufferAttachment(_pSrcTexVec.at(iLevel).get(), iFace, 0));
						_fbo->apply(*renderInfo.getState());

						GLint iW = _pSrcTexVec.at(iLevel)->getTextureWidth();
						GLint iH = _pSrcTexVec.at(iLevel)->getTextureHeight();

						switch (iFace)
						{
						case osg::TextureCubeMap::POSITIVE_X:
						{
							glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, iLevel, 0, 0, 0, 0, iW, iH);
						}
						break;
						case osg::TextureCubeMap::NEGATIVE_X:
						{
							glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, iLevel, 0, 0, 0, 0, iW, iH);
						}
						break;
						case osg::TextureCubeMap::POSITIVE_Y:
						{
							glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, iLevel, 0, 0, 0, 0, iW, iH);
						}
						break;
						case osg::TextureCubeMap::NEGATIVE_Y:
						{
							glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, iLevel, 0, 0, 0, 0, iW, iH);
						}
						break;
						case osg::TextureCubeMap::POSITIVE_Z:
						{
							glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, iLevel, 0, 0, 0, 0, iW, iH);
						}
						break;
						case osg::TextureCubeMap::NEGATIVE_Z:
						{
							glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, iLevel, 0, 0, 0, 0, iW, iH);
						}
						break;
						default:
							break;
						}
					}
				}
			}
		}

	private:
		osg::ref_ptr<osg::FrameBufferObject> _fbo;
		std::vector<osg::ref_ptr<osg::TextureCubeMap>> _pSrcTexVec;
		osg::ref_ptr<osg::TextureCubeMap> _pDstTex;
	};

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

void CGMMaterial::_InitProbe()
{
	// Create cubeMap texture for 6 direction
	int iCubeVectorSize = min(PROBE_MIPMAP_NUM + 1, int(log2f(PROBE_CUBESIZE)));//只需要0-6级的mipmap
	m_pCubeMapVector.reserve(iCubeVectorSize);
	for (int i = 0; i < iCubeVectorSize; i++)
	{
		int iSize = fmax(2.0f, PROBE_CUBESIZE * exp2f(-i));
		osg::ref_ptr<osg::TextureCubeMap> pCubeMapTex = new osg::TextureCubeMap();
		pCubeMapTex->setName("sceneCubeTex_" + std::to_string(i));
		pCubeMapTex->setInternalFormat(GL_RGBA8);
		pCubeMapTex->setSourceFormat(GL_RGBA);
		pCubeMapTex->setSourceType(GL_UNSIGNED_BYTE);
		pCubeMapTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
		pCubeMapTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
		pCubeMapTex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		pCubeMapTex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
		pCubeMapTex->setTextureSize(iSize, iSize);
		pCubeMapTex->setDataVariance(osg::Object::DYNAMIC);
		m_pCubeMapVector.push_back(pCubeMapTex);
	}

	// mipmap的计算和拷贝的根节点
	osg::ref_ptr<osg::Geode> pMipmapRootGeode = new osg::Geode();
	pMipmapRootGeode->setCullingActive(false);
	GM_Root->addChild(pMipmapRootGeode.get());

	// 计算mipmap的“compute shader”节点
	for (int iSourceLevel = 0; iSourceLevel < PROBE_MIPMAP_NUM; iSourceLevel++)
	{
		pMipmapRootGeode->addDrawable(_CreateMipmapCompute(iSourceLevel));
	}
	// 用于拷贝mipmap的节点
	//pMipmapRootGeode->addDrawable(_CreateMipmapCopyNode());
}

CGMDispatchCompute* CGMMaterial::_CreateMipmapCompute(const int iSourceLevel)
{
	osg::ref_ptr<osg::StateSet> pMipmapComputeSS = new osg::StateSet();
	pMipmapComputeSS->setRenderBinDetails(PROBE_MIPMAP_BIN + iSourceLevel, "RenderBin");

	// 图片单元不能重复，dirty没有成功，暂时不知道原因
	int iUnit = iSourceLevel;
	// 设置资源数据图
	CGMKit::AddImage(pMipmapComputeSS.get(), m_pCubeMapVector.at(iSourceLevel).get(), "sourceImg", iUnit++, osg::Texture::READ_ONLY, 0, 0, true);
	// 设置目标数据图
	CGMKit::AddImage(pMipmapComputeSS.get(), m_pCubeMapVector.at(iSourceLevel + 1).get(), "targetImg", iUnit++, osg::Texture::WRITE_ONLY, 0, 0, true);

	// 添加着色器，用于生成自定义mipmap
	std::string strCompPath = m_pConfigData->strCorePath + m_strModelShaderPath + "probe_mipmap.comp";
	CGMKit::LoadComputeShader(pMipmapComputeSS.get(), strCompPath, "probe_mipmap");
	// 用于选择每一层级mipmap的 compute shader
	pMipmapComputeSS->setDefine("MIPMAP_" + std::to_string(iSourceLevel));

	int iSize_mipmap = max(8, int(PROBE_CUBESIZE * exp2(-(iSourceLevel + 1))));// cube的第一层mipmap尺寸
	osg::ref_ptr<CGMDispatchCompute> pMipmapComputeNode = new CGMDispatchCompute(iSize_mipmap / 8, iSize_mipmap / 8, 1);
	pMipmapComputeNode->setCullingActive(false);
	pMipmapComputeNode->setStateSet(pMipmapComputeSS);
	m_pMipmapComputeVec.push_back(pMipmapComputeNode);
	return pMipmapComputeNode.get();
}

osg::Geometry* CGMMaterial::_CreateMipmapCopyNode()
{
	osg::ref_ptr<osg::StateSet> pSS = new osg::StateSet();
	pSS->setRenderBinDetails(PROBE_COPY_BIN, "RenderBin");

	m_pCopyMipmapGeom = new osg::Geometry();
	m_pCopyMipmapGeom->setUseDisplayList(false);
	//m_pCopyMipmapCB = new CopyMipmapCallback(m_pCubeMapVector, m_pMipCubeTex.get());
	//m_pCopyMipmapGeom->setDrawCallback(m_pCopyMipmapCB);
	m_pCopyMipmapGeom->setStateSet(pSS);
	return m_pCopyMipmapGeom.get();
}

bool CGMMaterial::_CreateProbe(const std::string& strInputFilePath, const std::string& strOutputFilePath)
{
	osg::ref_ptr<osg::Image> pImg = osgDB::readImageFile(strInputFilePath);
	if (!pImg.valid()) 	return false;

	std::uniform_int_distribution<> iDistribution = std::uniform_int_distribution<>(0, 10000);

	if (GL_FLOAT != pImg->getDataType() ||
		strInputFilePath.npos == strInputFilePath.find(".hdr") ||
		strOutputFilePath.npos == strOutputFilePath.find(".dds"))
		return false;

	// 输入的图片数据为float，hdr格式，输出格式为dds格式，RGBA byte，且自动生成全部mipmap

	// 指定输出图片的尺寸
	int iImgWidth = 2048;
	int iImgHeight = 1024;
	int iMipmapRoughnessMax = 7;
	int iMipmapNum = (int)std::log2(osg::minimum(iImgWidth, iImgHeight));
	osg::Image* pTargetImage = new osg::Image;
	osg::Image::MipmapDataType vMipmap;
	// 计算mipmap大小
	int iS = iImgWidth;
	int iT = iImgHeight;

	// 计算每个像素的颜色个数
	int iChannelPerPixel = 4;
	unsigned char* pInputData = pImg->data();
	unsigned int iTotalSize = 0;
	for (int i = 0; i <= iMipmapNum; ++i)
	{
		iTotalSize += (iS * iT) * iChannelPerPixel;

		iS >>= 1;
		iT >>= 1;

		if (iS < 1) iS = 1;
		if (iT < 1) iT = 1;

		vMipmap.push_back(iTotalSize);
	}
	unsigned char* pData = new unsigned char[iTotalSize];
	iS = iImgWidth;
	iT = iImgHeight;
	for (int iLevel = 0; iLevel <= iMipmapNum; iLevel++)
	{
		double fRoughness = std::fmin(1.0, double(iLevel) / double(iMipmapRoughnessMax));
		for (int t = 0; t < iT; t++)
		{
			concurrency::parallel_for(int(0), int(iS), [&](int s) // 多线程
				//for (int s = 0; s < iS; s++)
				{
					int iPixelAddr = s + t * iS;
					osg::Vec2f vTargetUV = _PixelAddress2UV(iPixelAddr, iS, iT);

					osg::Vec2d vViewDirXY = osg::Vec2d(osg::minimum(vTargetUV.x() * 4 - 1.0, 1.0), vTargetUV.y() * 2 - 1.0);
					if (vTargetUV.x() > 0.5)// 上半球面
					{
						vViewDirXY.x() = osg::maximum(vTargetUV.x() * 4 - 3.0, -1.0);
					}
					double fLen = vViewDirXY.length();
					if (fLen >= 1)
					{
						//保证在单位球面上
						vViewDirXY /= fLen;
						fLen = 1.0;
					}
					// 为了避免双抛物面贴图的接缝位置有缝隙，必须在接缝位置加0.25个像素的border
					// 1.先计算接缝处的0.25个像素代表多少弧度
					double fBorderAngle = 0.25 * std::acos((iT * 0.5 - 1) / (iT * 0.5));
					// 2.再将俯仰角向水平方向拉回0.25个像素的角度
					// 注意：由于使用双抛物面算法，这里不需要加sqrt
					// std::sqrt(1 - fLen * fLen);
					double fViewDirZ = 1 - fLen * fLen;
					double fPitchAngle = fmax(0.0, std::atan2(fViewDirZ, fLen) - fBorderAngle);
					if (vTargetUV.x() <= 0.5)// 下半球面
					{
						fPitchAngle *= -1;
					}
					fViewDirZ = std::tan(fPitchAngle) * fLen;
					// vDir方向，单位为：°，球面坐标系，x = 北偏东角度，y = 俯仰角
					osg::Vec2d vDir = osg::Vec2d(
						osg::RadiansToDegrees(std::atan2(vViewDirXY.x(), vViewDirXY.y())) + 180.0,
						osg::RadiansToDegrees(fPitchAngle));
					osg::Vec2f vUV = _Dir2UV(vDir);
					osg::Vec4f vColor = CGMKit::GetImageColor(pImg, vUV.x(), vUV.y(), true);
					if (0.0 < fRoughness)
					{
						osg::Vec3d vViewDir = osg::Vec3d(vViewDirXY, fViewDirZ);
						vViewDir.normalize();// 这里必须要归一化
						osg::Vec3d vViewRight;
						if (vViewDir != osg::Vec3d(0, 0, 1))// 防止两个向量平行
						{
							vViewRight = vViewDir ^ osg::Vec3d(0, 0, 1);
						}
						else
						{
							vViewRight = vViewDir ^ osg::Vec3d(1, 0, 0);
						}
						vViewRight.normalize();
						osg::Vec3d vViewUp = vViewRight ^ vViewDir;
						vViewUp.normalize();

						int iSampleNum = fRoughness * 8192;
						double fLatMin2Pole = exp2((fRoughness - 1) * (iMipmapRoughnessMax + 1)) * osg::PI_2;
						double fLatMin = osg::PI_2 - fLatMin2Pole;
						for (int i = 0; i < iSampleNum; i++)
						{
							double fLat = fLatMin + fLatMin2Pole * iDistribution(m_iRandom) / 1e4;
							double fY = std::sin(fLat);
							double fXZ = std::sqrtf(1 - fY * fY);

							double fLon = osg::PI * 2 * iDistribution(m_iRandom) / 1e4f;
							double fX = fXZ * std::sin(fLon);
							double fZ = fXZ * std::cos(fLon);
							osg::Vec3d vOffsetView = vViewRight * fX + vViewDir * fY + vViewUp * fZ;
							vOffsetView.normalize();
							osg::Vec2d vOffsetDir = osg::Vec2d(
								osg::RadiansToDegrees(std::atan2(vOffsetView.x(), vOffsetView.y())) + 180.0,
								osg::RadiansToDegrees(std::asin(vOffsetView.z())));
							osg::Vec2f vOffsetDirUV = _Dir2UV(vOffsetDir);
							vColor += CGMKit::GetImageColor(pImg, vOffsetDirUV.x(), vOffsetDirUV.y(), true);
						}
						vColor /= (iSampleNum + 1);
					}
					// RGB
					int iAddr = iPixelAddr * iChannelPerPixel;
					if (iLevel > 0)
					{
						iAddr += vMipmap.at(iLevel - 1);
					}
					float fIllum = fmax(fmax(vColor.r(), vColor.g()), vColor.b());
					if (0.0f == fIllum)
					{
						pData[iAddr] = (unsigned char)0;
						pData[iAddr + 1] = (unsigned char)0;
						pData[iAddr + 2] = (unsigned char)0;
						pData[iAddr + 3] = (unsigned char)0;
					}
					else
					{
						pData[iAddr] = _Float2UnsignedChar(vColor.r() / fIllum);
						pData[iAddr + 1] = _Float2UnsignedChar(vColor.g() / fIllum);
						pData[iAddr + 2] = _Float2UnsignedChar(vColor.b() / fIllum);
						pData[iAddr + 3] = _Float2UnsignedChar(sqrt(fIllum) / 4);
					}
				}
			); // end parallel_for
		}

		iS >>= 1;
		iT >>= 1;

		if (iS < 1) iS = 1;
		if (iT < 1) iT = 1;
	}

	pTargetImage->setImage( iImgWidth, iImgHeight, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, pData, osg::Image::USE_NEW_DELETE, 4);
	pTargetImage->setMipmapLevels(vMipmap);
	osgDB::writeImageFile(*pTargetImage, strOutputFilePath);

	return true;
}
