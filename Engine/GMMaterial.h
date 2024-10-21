//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2040, LT
/// All rights reserved.
///
/// @file		GMMaterial.h
/// @brief		GMEngine - Material manager
/// @version	1.0
/// @author		LiuTao
/// @date		2024.02.04
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommon.h"
#include "GMCommonUniform.h"
#include <osg/Texture2D>
#include <osg/Texture2DArray>
#include <osg/BufferIndexBinding>
#include <osg/Vec2>

namespace GM
{
	/*************************************************************************
	 Structs
	*************************************************************************/

	/*************************************************************************
	Class
	*************************************************************************/

	/*!
	*  @Class CGMMaterial
	*  @brief DOF manager for GM
	*/
	class CGMMaterial
	{
	// 函数
	public:
		/** @brief 构造 */
		CGMMaterial();
		/** @brief 析构 */
		~CGMMaterial();

		/** @brief 初始化 */
		bool Init(SGMConfigData* pConfigData, CGMCommonUniform* pCommonUniform);

		/** @brief 实体模型加载shader
		* @param pNode 需要修改材质的节点指针
		*/
		void SetModelShader(osg::Node* pNode);

	private:
		
		/**
		* @brief 像素地址 转 像素中心点的UV
		* @param iAddress		像素地址（逐像素排列，不是内存）
		* @param iW				图宽度
		* @param iH				图高度
		* @return osg::Vec2f	像素中心点的UV，(0.0, 1.0)
		*/
		inline osg::Vec2f _PixelAddress2UV(const unsigned int iPixelAddress, const int iW, const int iH) const
		{
			return osg::Vec2f(((iPixelAddress % iW) + 0.5f) / float(iW), ((iPixelAddress / iW) + 0.5f) / float(iH));
		}

	// 变量
	private:
		SGMConfigData*							m_pConfigData;							//!< 配置数据
		CGMCommonUniform*						m_pCommonUniform;						//!< 公共Uniform

		std::string								m_strModelShaderPath;					//!< 模型shader路径
		std::string								m_strDefTexPath;						//!< 模型添加贴图的默认路径

		osg::ref_ptr<osgDB::Options>			m_pDDSOptions;							//!< dds的纹理操作
		// 默认的各个材质的贴图，用于补齐纹理单元
		std::vector<osg::ref_ptr<osg::Texture2D>> m_pPBRTexVector;						//!< PBR模型的纹理单元默认贴图

		// 共用贴图
		osg::ref_ptr<osg::Texture2D>			m_pRainRippleTex;						//!< 水面涟漪
		osg::ref_ptr<osg::Texture2D>			m_pWetNormalTex;						//!< 潮湿表面法线贴图
		osg::ref_ptr<osg::Texture2D>			m_pNoiseTex;							//!< 噪声贴图
		osg::ref_ptr<osg::Texture2D>			m_pSnowTex;								//!< 积雪贴图
		osg::ref_ptr<osg::Texture2D>			m_pSandTex;								//!< 沙地贴图
	};

}	// GM
