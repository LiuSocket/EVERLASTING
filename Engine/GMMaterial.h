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
#include "GMKernel.h"
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
		bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData);
		/** @brief 更新(在主相机更新姿态之后) */
		bool UpdatePost(double dDeltaTime);
		/**
		* @brief 修改屏幕尺寸时调用此函数
		* @param width: 屏幕宽度
		* @param height: 屏幕高度
		*/
		void ResizeScreen(const int width, const int height);

		/** @brief 加载 PBR材质
		* @param pNode 需要修改材质的节点指针
		*/
		void SetPBRMaterial(osg::Node* pNode);
		/**
		* @brief 加载 人类（包括皮肤和眼睛）材质
		* @param pNode 需要修改材质的节点指针
		*/
		void SetHumanMaterial(osg::Node* pNode);
		/**
		* @brief 加载 次表面散射材质
		* @param pNode/pGeom 需要修改材质的节点指针
		*/
		void SetSSSMaterial(osg::Node* pNode);
		/**
		* @brief 加载 眼睛材质
		* @param pNode/pGeom 需要修改材质的节点指针
		*/
		void SetEyeMaterial(osg::Node* pNode);
		/**
		* @brief 加载 背景材质
		* @param pNode 需要修改材质的节点指针
		*/
		void SetBackgroundMaterial(osg::Node* pNode);

		/**
		* @brief 加载shader
		* @param pStateSet 需要添加shader的状态集
		* @param eMaterial 材质类型
		*/
		void SetShader(osg::StateSet* pStateSet, EGMMaterial eMaterial);

		/**
		* @brief 获取眼睛的变幻节点的vector
		* @param v: 有则返回节点指针的vector
		*/
		void GetEyeTransform(std::vector<osg::ref_ptr<osg::Transform>>& v) const
		{
			v = m_pEyeTransVector;
		}

	private:
		/**
		* @brief 判断当前纹理单元是否已经被占用，如果被占用，则++（跳过被占用的纹理单元）
		* @param iUnit 纹理单元
		* @return bool: 如果被占用则返回false，否则返回true
		*/
		bool _PlusUnitUsed(int& iUnit);
		/** @brief 初始化 SSS 模糊相机 */
		void _InitSSSBlur();

	// 变量
	private:
		SGMKernelData* m_pKernelData = nullptr;					//!< 内核数据
		SGMConfigData* m_pConfigData = nullptr;					//!< 配置数据

		std::string								m_strModelShaderPath;			//!< 模型shader路径
		std::string								m_strDefTexPath;				//!< 模型添加贴图的默认路径
		std::default_random_engine				m_iRandom;						//!< 随机值
		osg::ref_ptr<osgDB::Options>			m_pDDSOptions;					//!< dds的纹理操作
		// 默认的各个材质的贴图，用于补齐纹理单元
		std::vector<osg::ref_ptr<osg::Texture2D>> m_pPBRTexVector;				//!< PBR模型的纹理单元默认贴图
		// 共用贴图
		osg::ref_ptr<osg::Texture2D>			m_pRainRippleTex;				//!< 水面涟漪
		osg::ref_ptr<osg::Texture2D>			m_pWetNormalTex;				//!< 潮湿表面法线贴图
		osg::ref_ptr<osg::Texture2D>			m_pNoiseTex;					//!< 噪声贴图
		osg::ref_ptr<osg::Texture2D>			m_pSnowTex;						//!< 积雪贴图
		osg::ref_ptr<osg::Texture2D>			m_pSandTex;						//!< 沙地贴图
		osg::ref_ptr<osg::Texture2D>			m_pSkinDetailNormTex;			//!< 皮肤细节法线贴图
		osg::ref_ptr<osg::Texture2D>			m_pEnvProbeTex;					//!< 环境探针贴图

		std::vector<osg::ref_ptr<osg::TextureCubeMap>> m_pCubeMapVector;		//!< cubemap数组，6个方向6层level
		std::vector<osg::ref_ptr<CGMDispatchCompute>> m_pMipmapComputeVec;		//!< 生成自定义mipmap的计算着色器节点

		std::vector<osg::ref_ptr<osg::Transform>>	m_pEyeTransVector;			//!< 眼睛的变幻节点

		osg::ref_ptr<osg::Camera>				m_pSSSBlurCamera;				//!< SSS 模糊相机
		osg::ref_ptr<osg::Texture2D>			m_pSSSBlurTexture;				//!< SSS 模糊贴图
	};

}	// GM
