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

    // �ƹ��������
    #define GM_MAX_LIGHTNUM            (16)

    /*************************************************************************
     Enums
    *************************************************************************/
    /**
    * �ƹ�����ö��
    */
    enum EGMLIGHT_SOURCE
    {
        /** ���Դ */
        EGMLIGHT_SOURCE_POINT,
        /** �۹�� */
        EGMLIGHT_SOURCE_SPOT,
        /** ƽ�й�Դ */
        EGMLIGHT_SOURCE_DIRECTIONAL,
    };

    /*************************************************************************
     Structs
    *************************************************************************/

    /**
    * �ƹ����ݽṹ��
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

        /** �ƹ����� */
        std::string			strName;
        /** �ƹ����� */
        EGMLIGHT_SOURCE	eType;
        /** �ƹ�λ��(ECEF)������ռ䣬��λ���� */
        osg::Vec4d			vPos;
        /** �ƹⷽ��(ECEF)������ռ� */
        osg::Vec4d			vDir;
        /** �ƹ���ɫ */
        osg::Vec3			vColor;
        /** �����ǿ�ȣ���λ��cd���������� */
        float				fLuminous;
        /** �����ӳ��ǣ�FOV������λ���� */
        float				fAngle;
        /** �۹�̶ȣ��ο�openGL�Ĺ�Դ��spotExponent�� */
        float				fSpotExponent;
        /** �Ƿ������Ӱ */
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
        * @brief ���õƹ����
        * @params viewPos		��Դλ�ã�view�ռ䣩
        * @params viewDir		��Դ����view�ռ䣩
        * @params color			��Դ��ɫ
        * @params fAngle		�����Ƕȣ���λ����
        * @params spotExponent	�۹�̶ȣ��ο�openGL�Ĺ�Դ��spotExponent��
        * @params fRange		˥�����룬�ƹ�������������ƽ���ɷ��ȣ����������֮��ƹ��ܰ��������ڹ�ͻȻ��ʧ�ľ���
        * @params iID           �ƹ�ID
        */
        void SetLight(
            const osg::Vec3f& viewPos, const osg::Vec3f& viewDir, const osg::Vec3f& color,
            const float fAngle, const float spotExponent, const float fRange, const int iID)
        {
            if (iID >= GM_MAX_LIGHTNUM) return;
            viewPosAndCut[iID] = osg::Vec4f(viewPos, std::cos(osg::DegreesToRadians(fAngle)));
            viewDirAndSpotExponent[iID] = osg::Vec4f(viewDir, spotExponent);
            colorAndRange[iID] = osg::Vec4f(color, fRange);
        }

        // xyz = view�ռ��λ��, w = �����Ƕȵ�����ֵ��spotCosCutoff��
        osg::Vec4f		viewPosAndCut[GM_MAX_LIGHTNUM];
        // xyz = view�ռ�ķ���, w = �۹�̶ȣ��ο�openGL�Ĺ�Դ��spotExponent��
        osg::Vec4f		viewDirAndSpotExponent[GM_MAX_LIGHTNUM];
        // xyz = �ƹ���ɫ, w = ˥�����룬�ƹ�������������ƽ���ɷ��ȣ����������֮��ƹ��ܰ��������ڹ�ͻȻ��ʧ�ľ���
        osg::Vec4f		colorAndRange[GM_MAX_LIGHTNUM];
        // x = �ƹ�������yzw = padding
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
    // ����
    protected:
        /** @brief ���� */
        CGMLight();
        /** @brief ���� */
        virtual ~CGMLight();

    public:
        /** @brief ��ȡ���� */
        static CGMLight& getSingleton(void);

        /** @brief ��ʼ�� */
        bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData);
        /** @brief �ͷ� */
        void Release();
        /** @brief ���� */
        bool Update(double dDeltaTime);
        /** @brief ����(�������������̬֮��) */
        bool UpdatePost(double dDeltaTime);

        /** @brief ��� */
        bool Add(SLightData& sData);
        /** @brief �޸� */
        bool Edit(const SLightData& sData);
        /** @brief �Ƴ� */
        bool Remove(const std::string& strName);
        /** @brief ��� */
        bool Clear();
        /**
        * @brief ����ĳ�����Ƶĵƹ��������Ϣ
        * @param sData:		���еƹ����ƵĽṹ�壬����ɹ�������Ϣ��䵽�ṹ����
        * @return bool:		�Ƿ��ҵ������Ƶĵƹ�
        */
        bool Find(SLightData& sData);

        /**
        * @brief ���ö��Դ����
        * @param pNode:	��Ҫ��Ӷ��Դ�Ľڵ�
        * @param bEnable:	�Ƿ����ö��Դ
        * @return bool: �ɹ�����true��ʧ�ܷ���false
        */
        bool SetLightEnable(osg::Node* pNode, bool bEnable);

        /** @brief ������Ҫ��Ӱ�Ľڵ� */
        void SetShadowEnable(osg::Node* pNode);
        /** @brief ��ȡ��Ӱ��ͼ */
        inline osg::Texture2D* GetShadowMap() const
        {
            return m_pShadowTexture.get();
        }
        inline osg::Uniform* const GetView2ShadowMatrixUniform() const
        {
            return m_mView2ShadowUniform.get();
        }

    private:
        // ��Ӱ��ʼ��
        void _InitShadow();

    // ����
    private:
        SGMKernelData* m_pKernelData = nullptr;					//!< �ں�����
        SGMConfigData* m_pConfigData = nullptr;					//!< ��������

        //!< �ƹ��б�
        std::map<std::string, SLightData>                       m_mapLight;
        //!< �ƹ����ݵ�buffer
        osg::ref_ptr<osg::BufferTemplate<SLightDataBuffer>>     m_pLightBuffer;
        //!< �ƹ����ݵ�UniformBufferBinding
        osg::ref_ptr<osg::UniformBufferBinding>                 m_pLightUBB;

        //!< ��Ӱ���
        osg::ref_ptr<osg::Camera>				m_pShadowCamera;
        //!< ��Ӱ��ͼ
        osg::ref_ptr<osg::Texture2D>			m_pShadowTexture;
        //!< view�ռ�ת��Ӱ�ռ��Uniform
        osg::ref_ptr<osg::Uniform>				m_mView2ShadowUniform;
    };
}	// GM
