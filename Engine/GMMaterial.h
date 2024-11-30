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
#include <random>

namespace GM
{
	/*************************************************************************
	 Structs
	*************************************************************************/

	/*************************************************************************
	Class
	*************************************************************************/
	class CGMDispatchCompute;
	class CopyMipmapCallback;

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
		bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData, CGMCommonUniform* pCommonUniform);

		/** @brief 加载 PBR材质
		* @param pNode 需要修改材质的节点指针
		*/
		void SetPBRMaterial(osg::Node* pNode);

		/** @brief 加载 背景材质
		* @param pNode 需要修改材质的节点指针
		*/
		void SetBackgroundMaterial(osg::Node* pNode);

		/** @brief 加载 PBR shader
		*/
		void SetShader(osg::StateSet* pSS, EGMMaterial eMaterial);

	private:

	// 变量
	private:
		SGMKernelData* m_pKernelData = nullptr;					//!< 内核数据
		SGMConfigData* m_pConfigData = nullptr;					//!< 配置数据
		CGMCommonUniform* m_pCommonUniform = nullptr;			//!< 公共Uniform

		std::string								m_strModelShaderPath;					//!< 模型shader路径
		std::string								m_strDefTexPath;						//!< 模型添加贴图的默认路径
		std::default_random_engine				m_iRandom;								//!< 随机值
		osg::ref_ptr<osgDB::Options>			m_pDDSOptions;							//!< dds的纹理操作
		// 默认的各个材质的贴图，用于补齐纹理单元
		std::vector<osg::ref_ptr<osg::Texture2D>> m_pPBRTexVector;						//!< PBR模型的纹理单元默认贴图

		// 共用贴图
		osg::ref_ptr<osg::Texture2D>			m_pRainRippleTex;						//!< 水面涟漪
		osg::ref_ptr<osg::Texture2D>			m_pWetNormalTex;						//!< 潮湿表面法线贴图
		osg::ref_ptr<osg::Texture2D>			m_pNoiseTex;							//!< 噪声贴图
		osg::ref_ptr<osg::Texture2D>			m_pSnowTex;								//!< 积雪贴图
		osg::ref_ptr<osg::Texture2D>			m_pSandTex;								//!< 沙地贴图
		osg::ref_ptr<osg::Texture2D>			m_pEnvProbeTex;							//!< 环境探针贴图

		std::vector<osg::ref_ptr<osg::TextureCubeMap>> m_pCubeMapVector; //!< cubemap数组，6个方向6层level
		std::vector<osg::ref_ptr<CGMDispatchCompute>> m_pMipmapComputeVec; // 生成自定义mipmap的计算着色器节点
		osg::ref_ptr<osg::Geometry>				m_pCopyMipmapGeom;		// 拷贝mipmap的集合节点
		CopyMipmapCallback* m_pCopyMipmapCB = nullptr;	// 拷贝mipmap的回调

	};

}	// GM
