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

		/** @brief 加载 PBR shader
		* @param pNode 需要修改材质的节点指针
		*/
		void SetPBRShader(osg::Node* pNode);

		/** @brief 加载 背景 shader
		* @param pNode 需要修改材质的节点指针
		*/
		void SetBackgroundShader(osg::Node* pNode);

	private:
		/** @brief 初始化环境探针probe */
		void _InitProbe();
		/**
		* @brief 创建用于生成自定义mipmap的Compute节点
		* @param iSourceLevel: 源mipmap层级，只能是0、1、2、3、4
		*/
		CGMDispatchCompute* _CreateMipmapCompute(const int iSourceLevel);

		/** @brief 创建用于拷贝mipmap的节点 */
		osg::Geometry* _CreateMipmapCopyNode();
		/**
		* @brief 用全景图生成反射探针Probe
		* @param strInputFilePath		输入的全景图路径
		* @param strOutputFilePath		输出的probe图路径
		* @return bool					是否成功
		*/
		bool _CreateProbe( const std::string& strInputFilePath, const std::string& strOutputFilePath);

		/**
		* @brief 方向 转 全景图数据中的UV
		* @param vDir			方向，单位为：°，球面坐标系，x = 北偏东角度，y = 俯仰角
		* @return osg::Vec2f	全景图中的UV [0.0,1.0]
		*/
		inline osg::Vec2f _Dir2UV(const osg::Vec2f& vDir) const
		{
			return osg::Vec2f(vDir.x() / 360.0f, vDir.y() / 180.0f + 0.5f);
		}
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
		/**
		* @brief float转unsigned char
		* @param fIn			float输入,范围[0.0,1.0]
		* @return unsigned char	输出的8位数据，[0,255]的整数
		*/
		inline unsigned char _Float2UnsignedChar(const float fIn) const
		{
			float fX = 255.0f * fIn;
			if ((fX - int(fX)) >= 0.5f) { fX += 1.0; }
			return (unsigned char)(fmin(fX, 255.0f));
		}

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
