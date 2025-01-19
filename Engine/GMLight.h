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
     *  @brief GM Light Module
     */
    class CGMLight
    {
    // 函数
    public:
        /** @brief 构造 */
        CGMLight();
        /** @brief 析构 */
        ~CGMLight();

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

        /** @brief 增加需要阴影的节点 */
        void SetShadowEnable(osg::Node* pNode);
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
        void _InnerUpdate(const double dDeltaTime);
        // 阴影初始化
        void _InitShadow();

    // 变量
    private:
        SGMKernelData* m_pKernelData = nullptr;					//!< 内核数据
        SGMConfigData* m_pConfigData = nullptr;					//!< 配置数据

        //!< osg的射灯
        osg::ref_ptr<osg::LightSource>			m_pLightSource;
        //!< 阴影相机
        osg::ref_ptr<osg::Camera>				m_pShadowCamera;
        //!< 阴影贴图
        osg::ref_ptr<osg::Texture2D>			m_pShadowTexture;
        //!< view空间转阴影空间的Uniform
        osg::ref_ptr<osg::Uniform>				m_mView2ShadowUniform;
        //!< 默认灯光位置
        osg::Vec3d                              m_vLightPos = osg::Vec3d(1, -2, 1.5);
    };
}	// GM
