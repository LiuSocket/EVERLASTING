//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMLight.h
/// @brief		GMEngine - Light Module
/// @version	1.0
/// @author		LiuTao
/// @date		2024.10.21
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommon.h"
#include "GMKernel.h"

#include <osg/BufferTemplate>
#include <osg/BufferIndexBinding>

namespace GM
{
	/*************************************************************************
	 Macro Defines
	*************************************************************************/
	#define GM_LIGHT                    CGMLight::getSingleton()

	// 灯光最大数量
	#define GM_MAX_LIGHTNUM            (16)

	/*************************************************************************
	 Enums
	*************************************************************************/
	/**
	* 灯光类型枚举
	*/
	enum EGMLIGHT_SOURCE
	{
		/** 点光源 */
		EGMLIGHT_SOURCE_POINT,
		/** 聚光灯 */
		EGMLIGHT_SOURCE_SPOT,
		/** 平行光源 */
		EGMLIGHT_SOURCE_DIRECTIONAL,
	};

	/*************************************************************************
	 Structs
	*************************************************************************/

	/**
	* 灯光数据结构体
	*/
	struct SLightData
	{
		SLightData()
			: strName(""), eType(EGMLIGHT_SOURCE_SPOT),
			vPos(osg::Vec4d(0.0, 0.0, 0.0, 1.0)),
			vDir(osg::Vec4d(0.0, 1.0, 0.0, 0.0)),
			vColor(osg::Vec3(1.0f, 1.0f, 1.0f)),
			fLuminous(3e5f), fAngle(35.0f), fSpotExponent(25.0f),
			bShadow(false)
		{
		}

		/** 灯光名称 */
		std::string			strName;
		/** 灯光类型 */
		EGMLIGHT_SOURCE	eType;
		/** 灯光位置(ECEF)，世界空间，单位：米 */
		osg::Vec4d			vPos;
		/** 灯光方向(ECEF)，世界空间 */
		osg::Vec4d			vDir;
		/** 灯光颜色 */
		osg::Vec3			vColor;
		/** 额定发光强度（单位：cd，坎德拉） */
		float				fLuminous;
		/** 光束视场角（FOV），单位：° */
		float				fAngle;
		/** 聚光程度（参考openGL的光源的spotExponent） */
		float				fSpotExponent;
		/** 是否产生阴影 */
		bool				bShadow;
	};

	/* use UBO
	layout(std140) uniform LightDataBlock
	{
		vec4 viewPosAndCut[16]; // xyz = viewPos, w = spotCosCutoff
		vec4 viewDirAndSpotExponent[16]; // xyz = view light dir, w = spotExponent
		vec4 colorAndRange[16]; // xyz = light color, w = light fade range
		vec4 lightNum;// x = light num, yzw = padding
	}; */
	struct SLightDataBuffer
	{
		SLightDataBuffer()
		{
			for (int i = 0; i < GM_MAX_LIGHTNUM; ++i)
			{
				viewPosAndCut[i] = osg::Vec4f(0.0, 0.0, -10.0, 0.5);
				viewDirAndSpotExponent[i] = osg::Vec4f(0.0, 0.0, 1.0, 25.0);
				colorAndRange[i] = osg::Vec4f(1.0, 1.0, 1.0, 1000.0);
			}
		}

		/*
		* @brief 设置灯光参数
		* @params viewPos		光源位置（view空间）
		* @params viewDir		光源方向（view空间）
		* @params color			光源颜色
		* @params fAngle		光束角度，单位：°
		* @params spotExponent	聚光程度（参考openGL的光源的spotExponent）
		* @params fRange		衰减距离，灯光视亮度与距离的平方成反比，在这个距离之外灯光会很暗，不存在光突然消失的距离
		* @params iID           灯光ID
		*/
		void SetLight(
			const osg::Vec3f& viewPos, const osg::Vec3f& viewDir, const osg::Vec3f& color,
			const float fAngle, const float spotExponent, const float fRange, const int iID)
		{
			if (iID >= GM_MAX_LIGHTNUM) return;

			osg::Vec3f normViewDir = viewDir;
			normViewDir.normalize();

			viewPosAndCut[iID] = osg::Vec4f(viewPos, std::cos(osg::DegreesToRadians(fAngle)));
			viewDirAndSpotExponent[iID] = osg::Vec4f(normViewDir, spotExponent);
			colorAndRange[iID] = osg::Vec4f(color, fRange);
		}

		// xyz = view空间的位置, w = 光束角度的余弦值（spotCosCutoff）
		osg::Vec4f		viewPosAndCut[GM_MAX_LIGHTNUM];
		// xyz = view空间的方向, w = 聚光程度（参考openGL的光源的spotExponent）
		osg::Vec4f		viewDirAndSpotExponent[GM_MAX_LIGHTNUM];
		// xyz = 灯光颜色, w = 衰减距离，灯光视亮度与距离的平方成反比，在这个距离之外灯光会很暗，不存在光突然消失的距离
		osg::Vec4f		colorAndRange[GM_MAX_LIGHTNUM];
		// x = 灯光数量，yzw = padding
		osg::Vec4f		_lightNum = osg::Vec4f(.0f, .0f, .0f, 0.0f);
	};

	/*************************************************************************
	 Class
	*************************************************************************/

	/*!
	 *  @brief GM Light Module
	 */
	class CGMLight : public CGMSingleton<CGMLight>
	{
	// 函数
	protected:
		/** @brief 构造 */
		CGMLight();
		/** @brief 析构 */
		virtual ~CGMLight();

	public:
		/** @brief 获取单例 */
		static CGMLight& getSingleton(void);

		/** @brief 初始化 */
		bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData);
		/** @brief 释放 */
		void Release();
		/** @brief 更新 */
		bool Update(double dDeltaTime);
		/** @brief 更新(在主相机更新姿态之后) */
		bool UpdatePost(double dDeltaTime);

		/** @brief 添加 */
		bool Add(SLightData& sData);
		/** @brief 修改 */
		bool Edit(const SLightData& sData);
		/** @brief 移除 */
		bool Remove(const std::string& strName);
		/** @brief 清空 */
		bool Clear();
		/**
		* @brief 查找某个名称的灯光的所有信息
		* @param sData:		含有灯光名称的结构体，如果成功，则将信息填充到结构体里
		* @return bool:		是否找到该名称的灯光
		*/
		bool Find(SLightData& sData);

		/**
		* @brief 设置多光源光照
		* @param pNode:		需要添加多光源的节点
		* @param bEnable:	是否启用多光源
		* @return bool: 成功返回true，失败返回false
		*/
		bool SetLightEnable(osg::Node* pNode, bool bEnable);

		/**
		* @brief 设置节点是否投射阴影
		* @param pNode:		投射阴影的节点
		* @param bEnable:	是否投射阴影
		*/
		void SetCastShadowEnable(osg::Node* pNode,bool bEnable);
		/** @brief 获取阴影贴图 */
		inline osg::Texture2D* GetShadowMap() const
		{
			return m_pShadowTexture.get();
		}
		inline osg::Uniform* const GetView2ShadowMatrixUniform() const
		{
			return m_mView2ShadowUniform.get();
		}

	private:
		// 阴影初始化
		void _InitShadow();

	// 变量
	private:
		SGMKernelData* m_pKernelData = nullptr;					//!< 内核数据
		SGMConfigData* m_pConfigData = nullptr;					//!< 配置数据

		//!< 灯光列表
		std::map<std::string, SLightData>                       m_mapLight;
		//!< 灯光数据的buffer
		osg::ref_ptr<osg::BufferTemplate<SLightDataBuffer>>     m_pLightBuffer;
		//!< 灯光数据的UniformBufferBinding
		osg::ref_ptr<osg::UniformBufferBinding>                 m_pLightUBB;

		//!< 阴影相机
		osg::ref_ptr<osg::Camera>				m_pShadowCamera;
		//!< 阴影贴图
		osg::ref_ptr<osg::Texture2D>			m_pShadowTexture;
		//!< view空间转阴影空间的Uniform
		osg::ref_ptr<osg::Uniform>				m_mView2ShadowUniform;
	};
}	// GM
