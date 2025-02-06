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
     *  @class CGMModel��ϵͳ��λ������
     *  @brief GM Model Module
     */
    class CGMModel
    {
    // ����
    public:
        /** @brief ���� */
        CGMModel();
        /** @brief ���� */
        ~CGMModel();

        /** @brief ��ʼ�� */
        bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData);
        /** @brief ���� */
        bool Load();
        /** @brief ���� */
        bool Save();
        /** @brief ���� */
        bool Reset();
        /** @brief ���� */
        bool Update(double dDeltaTime);
        /** @brief ����(�������������̬֮��) */
        bool UpdatePost(double dDeltaTime);
        /** @brief ���ģ�� */
        bool Add(const SGMModelData& sData);

        /**
        * @brief ������߽���ģ�͵Ķ������ܣ��������������ζ�����
        * @param strName: ģ���ڳ����е�����
        * @param bEnable: �Ƿ����ö�������
        * @return bool �ɹ�OK��ʧ��Fail��ģ�Ͳ����ڷ���NotExist
        */
        bool SetAnimationEnable(const std::string& strName, const bool bEnable);
        /**
        * @brief ��ȡָ��ģ�͵Ķ������ܣ��������������ζ������Ƿ���
        * @param strName: ģ���ڳ����е�����
        * @return bool: �Ƿ����ö���
        */
        bool GetAnimationEnable(const std::string& strName);

    private:
        /**
        * @brief ���ز���
        * @param pNode ��Ҫ�޸Ĳ��ʵĽڵ�ָ��
        * @param sData ��Ҫ�޸ĵ�ģ����Ϣ
        * @return bool �ɹ����� true��ʧ�ܷ��� true
        */
        bool _SetMaterial(osg::Node* pNode, const SGMModelData& sData);
    
        /**
        * @brief �������ƻ�ȡģ��
        * @param strModelName ģ������
        * @return osg::Node* ģ�ͽڵ�ָ��
        */
        osg::Node* _GetNode(const std::string& strModelName);

        void _InnerUpdate(const double dDeltaTime);

    // ����
    private:
        SGMKernelData* m_pKernelData = nullptr;					//!< �ں�����
        SGMConfigData* m_pConfigData = nullptr;					//!< ��������

        osg::ref_ptr<osg::Group>			m_pRootNode = nullptr;
        std::map<std::string, SGMModelData>	m_pModelDataMap;	//!< ģ������map
        std::map<std::string, osg::ref_ptr<osg::Node>> m_pNodeMap;	//!< ģ�ͽڵ�map

        // �����ͼ��Ĭ��·��
        std::string							m_strDefTexPath = "Textures/";
        // ģ���ļ�Ĭ��·��
        std::string							m_strDefModelPath = "Models/";
        //!< dds���������
        osg::ref_ptr<osgDB::Options>		m_pDDSOptions;
        //!< ���ʹ�����
        CGMMaterial*						m_pMaterial = nullptr;
    };
}	// GM
