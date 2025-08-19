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

namespace GM
{
	/*************************************************************************
	 Enums
	*************************************************************************/

	/*************************************************************************
	 Structs
	*************************************************************************/

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

		/** @brief 创建地形几何体 */
		osg::Geometry* _CreateTerrainGeometry() const;

		/**
		* @brief 创建对应层级瓦片的几何体，每个顶点都有法线和UV
		* UV0.xy = WGS84对应的UV，[0.0, 1.0]
		* @param iTileVec:		瓦片的各级数据
		* @param iSegment:		瓦片体的边长的分段数，建议设置成2^n-1是为了保证高程图的分辨率是2^n
		* @return Geometry:		返回几何体指针
		*/
		osg::Geometry* _MakeTileGeometry(const std::vector<int>& iTileVec, int iSegment) const;

	// 变量
	private:
		SGMKernelData* m_pKernelData = nullptr;					//!< 内核数据
		SGMConfigData* m_pConfigData = nullptr;					//!< 配置数据

		osg::ref_ptr<osg::Group>			m_pRootNode = nullptr;

		// 地形文件默认路径
		std::string							m_strTerrainPath = "Terrain/";
		//!< dds的纹理操作
		osg::ref_ptr<osgDB::Options>		m_pDDSOptions;
	};
}	// GM
