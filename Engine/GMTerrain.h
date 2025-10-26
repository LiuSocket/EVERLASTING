//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2025~2045, LiuTao
/// All rights reserved.
///
/// @file		GMTerrain.h
/// @brief		GMEngine - Terrain Module
/// @version	1.0
/// @author		LiuTao
/// @date		2025.08.17
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommon.h"
#include "GMKernel.h"
#include <osg/MatrixTransform>
#include <osg/BufferTemplate>
#include <osg/BufferIndexBinding>

namespace GM
{
	/*************************************************************************
	 Macro Defines
	*************************************************************************/

	// 方形积木的分段数
	#define BLOCK_SEGMENT					(127)
	// 环状地形的预计圈数（不包含中心区域）
	#define TERRAIN_RING_NUM				(7)
	// 地形的“M*M方形积木”的最大数量
	#define TERRAIN_RECT_MAX				(TERRAIN_RING_NUM * 2 + 2)

	/*************************************************************************
	 Enums
	*************************************************************************/

	/*************************************************************************
	 Structs
	*************************************************************************/

	/* use UBO
	layout(std140) uniform TerrainBlock
	{
		vec4 rectPosAndScale[TERRAIN_RECT_MAX];
	}; */
	struct STerrainBuffer
	{
		STerrainBuffer()
		{
			for (int i = 0; i < TERRAIN_RECT_MAX; ++i)
			{
				rectPosAndScale[i] = osg::Vec4f(0.0, 0.0, 0.0, 0.0);
			}
		}

		/*
		* @brief 设置条状积木的位置
		* @param iInstanceID:	积木的实例ID
		* @param fScale:		缩放比例，0 表示隐藏
		* @param vPos:			积木零点的偏移位置
		*/
		void SetRectanglePosAndScale(int iInstanceID, float fScale, const osg::Vec2f& vPos = osg::Vec2f(0,0))
		{
			rectPosAndScale[iInstanceID] = osg::Vec4f(vPos.x(), vPos.y(), fScale, 0.0f);
		}

		// xy = 每一块方形积木的位置
		// z = 缩放（0表示不渲染）
		// w待定
		osg::Vec4f	rectPosAndScale[TERRAIN_RECT_MAX];
	};

	/*************************************************************************
	 Class
	*************************************************************************/

	/*!
	 *  @class CGMTerrain，系统单位：厘米
	 *  @brief GM Terrain Module
	 */
	class CGMTerrain
	{
	// 函数
	public:
		/** @brief 构造 */
		CGMTerrain();
		/** @brief 析构 */
		~CGMTerrain();

		/** @brief 初始化 */
		bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData);
		/** @brief 加载 */
		bool Load();
		/** @brief 保存 */
		bool Save();
		/** @brief 重置 */
		bool Reset();
		/** @brief 更新 */
		bool Update(double dDeltaTime);
		/** @brief 更新(在主相机更新姿态之后) */
		bool UpdatePost(double dDeltaTime);
		/**
		* @brief 修改屏幕尺寸时调用此函数
		* @param width: 屏幕宽度
		* @param height: 屏幕高度
		*/
		void ResizeScreen(const int width, const int height);

	private:

		void _InnerUpdate(const double dDeltaTime);

		/**
		* @brief 创建Block方块几何体
		* @param iSegment:		瓦片体的边长的分段数，建议设置成2^n-1是为了保证高程图的分辨率是2^n
		* @return Geometry:		返回几何体指针
		*/
		osg::Geometry* _MakeTerrainBlockGeometry(int iSegment) const;

	// 变量
	private:
		SGMKernelData* m_pKernelData = nullptr;					//!< 内核数据
		SGMConfigData* m_pConfigData = nullptr;					//!< 配置数据

		osg::ref_ptr<osg::Group>			m_pRootNode = nullptr;
		osg::ref_ptr<osg::MatrixTransform>	m_pTerrainTrans = nullptr;
		osg::ref_ptr<osg::Geode>			m_pTerrainGeode = nullptr;
		// 地形shader路径
		std::string							m_strTerrainShaderPath = "Shaders/TerrainShader/";
		// 地形贴图路径
		std::string							m_strTerrainTexPath = "Textures/TerrainTexture/";
		// dds的纹理操作
		osg::ref_ptr<osgDB::Options>		m_pDDSOptions;
		// 眼点位置
		osg::Vec3d							m_vEyePos = osg::Vec3d(0.0, 0.0, 0.0);
		// 视线方向
		osg::Vec3d							m_vViewDir = osg::Vec3d(0, 1, 0);
		// 中心圈层（0层和1层）网格分辨率，单位：cm
		float								m_fMinSegSize = 1.0f;
		//!< 灯光数据的buffer
		osg::ref_ptr<osg::BufferTemplate<STerrainBuffer>>		m_pTerrSRTBuffer;
		//!< 灯光数据的UniformBufferBinding
		osg::ref_ptr<osg::UniformBufferBinding>					m_pTerrSRTUBB;
	};
}	// GM
