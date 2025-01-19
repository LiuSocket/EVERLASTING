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
#include "GMCommonUniform.h"
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

    class CGMAnimation;
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
        bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData, CGMCommonUniform* pCommonUniform);
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

        /* @brief 传递Uniform */
        void SetUniform(osg::Uniform* pView2Shadow);

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

        /**
        * @brief 设置动画播放时长
        * @param strModelName 模型名称
        * @param fDuration 动画时长，单位：秒
        * @param strAnimationName 动画名称，不输入动画名称就只修改第一个或者上一个控制的动画
        * @return bool 成功返回 true，失败返回 false
        */
        bool SetAnimationDuration(const std::string& strModelName, const float fDuration, const std::string& strAnimationName = "");
        /**
        * @brief 获取动画播放时长
        * @param strModelName 模型名称
        * @param strAnimationName 动画名称
        * @return float 动画时长，单位：秒
        */
        float GetAnimationDuration(const std::string& strModelName, const std::string& strAnimationName);
        /**
        * @brief 设置动画播放模式
        * @param strModelName 模型名称
        * @param ePlayMode 播放模式
        * @param strAnimationName 动画名称，不输入动画名称就只修改第一个或者上一个控制的动画
        * @return bool 成功返回 true，失败返回 false
        */
        bool SetAnimationMode(const std::string& strModelName, EGMPlayMode ePlayMode, const std::string& strAnimationName = "");
        /**
        * @brief 获取动画播放模式
        * @param strModelName 模型名称
        * @param strAnimationName 动画名称
        * @return EGMPlayMode 播放模式
        */
        EGMPlayMode GetAnimationMode(const std::string& strModelName, const std::string& strAnimationName);
        /**
        * @brief 设置动画优先级
        * @param strModelName 模型名称
        * @param iPriority 动画优先级，[0,100]，数值越大优先级越高
        * @param strAnimationName 动画名称，不输入动画名称就只修改第一个或者上一个控制的动画
        * @return bool 成功返回 true，失败返回 false
        */
        bool SetAnimationPriority(const std::string& strModelName, const int iPriority, const std::string& strAnimationName = "");
        /**
        * @brief 获取动画优先级
        * @param strModelName 模型名称
        * @param strAnimationName 动画名称
        * @return int 动画优先级
        */
        int GetAnimationPriority(const std::string& strModelName, const std::string& strAnimationName);
        /**
        * @brief 播放动画，如果不输入动画名称且输入权重为0.0，就停止所有动画
        * @param strModelName 模型名称
        * @param fWeight 动画混合权重，[0.0,1.0]，0.0表示停止，1.0表示播放，动画可根据权重混合
        * @param strAnimationName 动画名称，不输入动画名称就播放第一个或者上一个控制的动画
        * @return bool 成功返回 true，失败返回 false
        */
        bool SetAnimationPlay(const std::string& strModelName, const float fWeight, const std::string& strAnimationName = "");
        /**
        * @brief 暂停动画
        * @param strModelName 模型名称
        * @param strAnimationName 动画名称，不输入动画名称就暂停所有动画
        * @return bool 成功返回 true，失败返回 false
        */
        bool SetAnimationPause(const std::string& strModelName, const std::string& strAnimationName = "");
        /**
        * @brief 继续播放动画
        * @param strModelName 模型名称
        * @param strAnimationName 动画名称，不输入动画名称就播放第一个或者上一个控制的动画
        * @return bool 成功返回 true，失败返回 false
        */
        bool SetAnimationResume(const std::string& strModelName, const std::string& strAnimationName = "");

        /** @brief 获取阴影模型根节点 */
        inline osg::Node* GetRootNode() const
        {
            return m_pRootNode.get();
        }
        /* @brief 设置阴影图 */
        void SetShadowMap(osg::Texture2D* pShadowMap);

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
        CGMCommonUniform* m_pCommonUniform = nullptr;			//!< 公共Uniform

        osg::ref_ptr<osg::Group>			m_pRootNode;
        std::map<std::string, SGMModelData>	m_pModelDataMap;	//!< 模型数据map
        std::map<std::string, osg::ref_ptr<osg::Node>> m_pNodeMap;	//!< 模型节点map

        // 添加贴图的默认路径
        std::string							m_strDefTexPath = "Textures/";
        // 模型文件默认路径
        std::string							m_strDefModelPath = "Models/";
        //!< dds的纹理操作
        osg::ref_ptr<osgDB::Options>		m_pDDSOptions;
        //!< 动画管理器
        CGMAnimation*						m_pAnimationManager;
        //!< 材质管理器
        CGMMaterial*						m_pMaterial;
    };
}	// GM
