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
	* @brief ģ�Ͳ��ʵ�drawable�������������Զ�����ĩβ��δ���ӵ�������Ԫ
	* ��ģ���������̣�����ģ�Ϳ���ֻ������Ҫ����ͼ������������ĳһ��������Ԫ���Ӻ����������Ԫ
	* ��ȷ������Ԫ��ϣ�0��0+1��0+1+2, 0+1+2+3��0+1+2+3+4
	* ����������Ԫ��ϣ�0+2+3��2+4...
	*/
	class AutoTexVisitor : public osg::NodeVisitor
	{
	public:
		AutoTexVisitor(std::vector<osg::ref_ptr<osg::Texture2D>> vTex) : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
			_vTex(vTex)
		{}

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
					// ��ȡ������Ҫ��������Ԫ
					std::vector<osg::StateAttribute*> vAttr;
					for (int i = 0; i < _vTex.size(); i++)
					{
						osg::StateAttribute* pAttr = pStateSet->getTextureAttribute(i, osg::StateAttribute::TEXTURE);
						vAttr.push_back(pAttr);
					}
					// �����ȱ��������Ԫ
					for (int i = 1; i < _vTex.size(); i++)
					{
						if (vAttr.at(i) || !_vTex.at(i).valid()) continue;
						pStateSet->setTextureAttributeAndModes(i, _vTex.at(i).get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
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

CGMMaterial::CGMMaterial(): m_pConfigData(nullptr), 
	m_strModelShaderPath("/modelShader/"), m_strDefTexPath("Resources/modelDefaultTexture/")
{
}

CGMMaterial::~CGMMaterial()
{
}

bool CGMMaterial::Init(SGMConfigData* pConfigData)
{
	m_pConfigData = pConfigData;

	// ��ʼ��Ĭ�ϵĸ������ʵ���ͼ�����ڲ���������Ԫ
	// ��ɫ��ͼ
	osg::ref_ptr<osg::Texture2D> pWhiteTex = new osg::Texture2D;
	pWhiteTex->setImage(osgDB::readImageFile(m_strDefTexPath + "white.tga"));
	pWhiteTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture2D::NEAREST);
	pWhiteTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture2D::NEAREST);
	pWhiteTex->setWrap(osg::Texture::WRAP_S, osg::Texture2D::REPEAT);
	pWhiteTex->setWrap(osg::Texture::WRAP_T, osg::Texture2D::REPEAT);
	pWhiteTex->setInternalFormat(GL_RGBA8);
	pWhiteTex->setSourceFormat(GL_RGBA);
	pWhiteTex->setSourceType(GL_UNSIGNED_BYTE);
	// ��ɫ��ͼ
	osg::ref_ptr<osg::Texture2D> pBlackTex = new osg::Texture2D;
	pBlackTex->setImage(osgDB::readImageFile(m_strDefTexPath + "black.tga"));
	pBlackTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture2D::NEAREST);
	pBlackTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture2D::NEAREST);
	pBlackTex->setWrap(osg::Texture::WRAP_S, osg::Texture2D::REPEAT);
	pBlackTex->setWrap(osg::Texture::WRAP_T, osg::Texture2D::REPEAT);
	pBlackTex->setInternalFormat(GL_RGBA8);
	pBlackTex->setSourceFormat(GL_RGBA);
	pBlackTex->setSourceType(GL_UNSIGNED_BYTE);
	// Ĭ��MRA��ͼ
	osg::ref_ptr<osg::Texture2D> pMRATex = new osg::Texture2D;
	pMRATex->setImage(osgDB::readImageFile(m_strDefTexPath + "default_MRA.tga"));
	pMRATex->setFilter(osg::Texture::MIN_FILTER, osg::Texture2D::NEAREST);
	pMRATex->setFilter(osg::Texture::MAG_FILTER, osg::Texture2D::NEAREST);
	pMRATex->setWrap(osg::Texture::WRAP_S, osg::Texture2D::REPEAT);
	pMRATex->setWrap(osg::Texture::WRAP_T, osg::Texture2D::REPEAT);
	pMRATex->setInternalFormat(GL_RGBA8);
	pMRATex->setSourceFormat(GL_RGBA);
	pMRATex->setSourceType(GL_UNSIGNED_BYTE);
	// Ĭ�Ϸ�����ͼ
	osg::ref_ptr<osg::Texture2D> pNormalTex = new osg::Texture2D;
	pNormalTex->setImage(osgDB::readImageFile(m_strDefTexPath + "default_n.tga"));
	pNormalTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture2D::NEAREST);
	pNormalTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture2D::NEAREST);
	pNormalTex->setWrap(osg::Texture::WRAP_S, osg::Texture2D::REPEAT);
	pNormalTex->setWrap(osg::Texture::WRAP_T, osg::Texture2D::REPEAT);
	pNormalTex->setInternalFormat(GL_RGBA8);
	pNormalTex->setSourceFormat(GL_RGBA);
	pNormalTex->setSourceType(GL_UNSIGNED_BYTE);
	
	//!< PBRģ�͵�������ԪĬ����ͼ
	m_pPBRTexVector.push_back(pWhiteTex); // 0 ������ɫ
	m_pPBRTexVector.push_back(pMRATex); // 1 �����ȡ��ֲڶȡ�AO
	m_pPBRTexVector.push_back(pBlackTex); // 2 �Է�����ͼ
	m_pPBRTexVector.push_back(pNormalTex); // 3 ������ͼ

	// ��ʼ�����й���ͼƬ��Դ
	// ˮ������
	m_pRainRippleTex = new osg::Texture2D;
	m_pRainRippleTex->setImage(osgDB::readImageFile(m_strDefTexPath + "ripple.tga"));
	m_pRainRippleTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pRainRippleTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pRainRippleTex->setWrap(osg::Texture::WRAP_S, osg::Texture2D::REPEAT);
	m_pRainRippleTex->setWrap(osg::Texture::WRAP_T, osg::Texture2D::REPEAT);
	m_pRainRippleTex->setInternalFormat(GL_RGBA8);
	m_pRainRippleTex->setSourceFormat(GL_RGBA);
	m_pRainRippleTex->setSourceType(GL_UNSIGNED_BYTE);
	// ��ʪ���淨����ͼ
	m_pWetNormalTex = new osg::Texture2D;
	m_pWetNormalTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pWetNormalTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pWetNormalTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
	m_pWetNormalTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
	m_pWetNormalTex->setImage(osgDB::readImageFile(m_strDefTexPath + "custom_n.dds"));
	// ������ͼ
	m_pNoiseTex = new osg::Texture2D;
	m_pNoiseTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pNoiseTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pNoiseTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
	m_pNoiseTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
	m_pNoiseTex->setImage(osgDB::readImageFile(m_strDefTexPath + "custom_noise.dds"));
	// ѩ��˪����ͼ
	m_pSnowTex = new osg::Texture2D;
	m_pSnowTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pSnowTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pSnowTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
	m_pSnowTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
	m_pSnowTex->setImage(osgDB::readImageFile(m_strDefTexPath + "snow.dds"));
	// ��ɳ��ͼ
	m_pSandTex = new osg::Texture2D;
	m_pSandTex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pSandTex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	m_pSandTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
	m_pSandTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
	m_pSandTex->setImage(osgDB::readImageFile(m_strDefTexPath + "sand.dds"));

	return true;
}

void CGMMaterial::SetModelShader(osg::Node* pNode)
{
	//!< �Զ��������ģ�͵�������Ԫ
	AutoTexVisitor cAutoTexVisitor(m_pPBRTexVector);
	pNode->accept(cAutoTexVisitor);

	osg::StateSet* pStateSet = new osg::StateSet();
	// ����shader
	std::string strShaderPath = m_pConfigData->strCorePath + m_strModelShaderPath;
	CGMKit::LoadShaderWithCommonFrag(pStateSet,
		strShaderPath + "GMModel.vert",
		strShaderPath + "GMModel.frag",
		strShaderPath + "GMCommon.frag",
		"Model");

	// Uniform
	int iChannel = 0;
	// ��������ͼ��RGBͨ�������ɫ��Aͨ����Ų�͸���ȣ�
	osg::ref_ptr<osg::Uniform> pTexBaseColorUniform = new osg::Uniform("texBaseColor", iChannel++);//������Ԫ��Ҫ�ҷ��䣡
	pStateSet->addUniform(pTexBaseColorUniform.get());

	// �Է���ǿ�� [0.0,1.0]
	osg::ref_ptr<osg::Uniform> pIllumUniform = new osg::Uniform("illumIntense", 1.0f);
	pStateSet->addUniform(pIllumUniform.get());

	// PBR��ͼ��Rͨ����Ž����ȣ�Gͨ����Ŵֲڶȣ�Bͨ�����AO��ͼ��Aͨ��������
	osg::ref_ptr<osg::Uniform> pTexPbrUniform = new osg::Uniform("texPbr", iChannel++);
	pStateSet->addUniform(pTexPbrUniform.get());
	// �Է�����ͼ��RGBͨ�������ɫ��Aͨ��������
	osg::ref_ptr<osg::Uniform> pTexIlluminationUniform = new osg::Uniform("texIllumination", iChannel++);
	pStateSet->addUniform(pTexIlluminationUniform.get());
	// ������ͼ��RGBͨ����ŷ��ߣ�Aͨ��������
	osg::ref_ptr<osg::Uniform> pTexNormalUniform = new osg::Uniform("texNormal", iChannel++);
	pStateSet->addUniform(pTexNormalUniform.get());

	pNode->setStateSet(pStateSet);
}