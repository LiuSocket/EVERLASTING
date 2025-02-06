//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMModel.h
/// @brief		GMEngine - Model Module
/// @version	1.0
/// @author		LiuTao
/// @date		2024.10.12
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommon.h"
#include "GMKernel.h"

#include <osg/Texture2D>

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

    class CGMMaterial;

    /*!
     *  @class CGMModel，系统单位：厘米
     *  @brief GM Model Module
     */
    class CGMModel
    {
    // 函数
    public:
        /** @brief 构造 */
        CGMModel();
        /** @brief 析构 */
        ~CGMModel();

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
        /** @brief 添加模型 */
        bool Add(const SGMModelData& sData);

        /**
        * @brief 激活或者禁用模型的动画功能（骨骼动画、变形动画）
        * @param strName: 模型在场景中的名称
        * @param bEnable: 是否启用动画功能
        * @return bool 成功OK，失败Fail，模型不存在返回NotExist
        */
        bool SetAnimationEnable(const std::string& strName, const bool bEnable);
        /**
        * @brief 获取指定模型的动画功能（骨骼动画、变形动画）是否开启
        * @param strName: 模型在场景中的名称
        * @return bool: 是否启用动画
        */
        bool GetAnimationEnable(const std::string& strName);

    private:
        /**
        * @brief 加载材质
        * @param pNode 需要修改材质的节点指针
        * @param sData 需要修改的模型信息
        * @return bool 成功返回 true，失败返回 true
        */
        bool _SetMaterial(osg::Node* pNode, const SGMModelData& sData);
    
        /**
        * @brief 根据名称获取模型
        * @param strModelName 模型名称
        * @return osg::Node* 模型节点指针
        */
        osg::Node* _GetNode(const std::string& strModelName);

        void _InnerUpdate(const double dDeltaTime);

    // 变量
    private:
        SGMKernelData* m_pKernelData = nullptr;					//!< 内核数据
        SGMConfigData* m_pConfigData = nullptr;					//!< 配置数据

        osg::ref_ptr<osg::Group>			m_pRootNode = nullptr;
        std::map<std::string, SGMModelData>	m_pModelDataMap;	//!< 模型数据map
        std::map<std::string, osg::ref_ptr<osg::Node>> m_pNodeMap;	//!< 模型节点map

        // 添加贴图的默认路径
        std::string							m_strDefTexPath = "Textures/";
        // 模型文件默认路径
        std::string							m_strDefModelPath = "Models/";
        //!< dds的纹理操作
        osg::ref_ptr<osgDB::Options>		m_pDDSOptions;
        //!< 材质管理器
        CGMMaterial*						m_pMaterial = nullptr;
    };
}	// GM
