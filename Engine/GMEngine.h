//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMVolumeBasic.h
/// @brief		Galaxy-Music Engine - GMEngine.h
/// @version	1.0
/// @author		LiuTao
/// @date		2020.12.10
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "GMCommon.h"
#include "GMKernel.h"
#include <random>

/*************************************************************************
Class
*************************************************************************/
class QWidget;

namespace GM
{
	/*************************************************************************
	Macro Defines
	*************************************************************************/
	#define GM_ENGINE					CGMEngine::getSingleton()

	/*************************************************************************
	Enums
	*************************************************************************/

	/*************************************************************************
	Structs
	*************************************************************************/

	/*************************************************************************
	Class
	*************************************************************************/

	class CGMPost;
	class CGMModel;
	class CGMCharacter;
	class CGMBaseManipulator;
	class CGMAudio;

	/*!
	*  @class CGMEngine
	*  @brief Galaxy-Music GMEngine
	*/
	class CGMEngine : public CGMSingleton<CGMEngine>
	{
		// ����
	protected:
		/** @brief ���� */
		CGMEngine();
		/** @brief ���� */
		~CGMEngine();

	public:
		/** @brief ��ȡ���� */
		static CGMEngine& getSingleton(void);

		/** @brief ��ʼ�� */
		bool Init();
		/** @brief �ͷ� */
		void Release();
		/** @brief ���� */
		bool Update();
		/** @brief ���� */
		bool Load();
		/** @brief ���� */
		bool Save();
		/**
		* @brief �޸���Ļ�ߴ�ʱ���ô˺���
		* @param iW: ��Ļ���
		* @param iH: ��Ļ�߶�
		*/
		void ResizeScreen(const int iW, const int iH);
		/**
		* @brief ��ʾ/����Ŀ��
		* @param bVisible ��ʾ/����
		*/
		void SetLookTargetVisible(bool bVisible);
		/**
		* @brief ����ע��Ŀ��λ��
		* @param vTargetScreenPos Ŀ������ĻXY����
		*/
		void SetLookTargetPos(const osg::Vec2f& vTargetScreenPos);
		/**
		* @brief ����/��ͣ��Ⱦ����С��������������ö�ʱ���Թر���Ⱦ
		*  �ر���Ϊ����С��ʱ���˷��Կ���Դ
		* @param bEnable: �Ƿ�����Ⱦ
		*/
		inline void SetRendering(const bool bEnable)
		{
			m_bRendering = bEnable;
		}
		/**
		* @brief �Ƿ�����Ⱦ
		* @return bool: �Ƿ�����Ⱦ
		*/
		inline bool GetRendering() const
		{
			return m_bRendering;
		}
		
		/** @brief �����ӿ�(QT:QWidget) */
		CGMViewWidget* CreateViewWidget(QWidget* parent);

	private:
		/**
		* @brief ��������
		*/
		bool _LoadConfig();
		/**
		* @brief ��ʼ��������ؽڵ�
		*/
		void _InitBackground();
		/**
		* @brief ��ʼ��ǰ����ؽڵ�
		*/
		void _InitForeground();
		
		/**
		* @brief ������£�һ���Ӹ���10��
		* @param updateStep ���μ�����µ�ʱ����λs
		*/
		void _InnerUpdate(const float updateStep);
		/** @brief ����(�������������̬֮��) */
		bool _UpdateLater(const double dDeltaTime);
		
		/**
		* @brief SGMVector3 ת osg::Vec3d
		* @param vGM:				�����GM����
		* @return osg::Vec3d:		�����osg���� */
		inline SGMVector3 _OSG2GM(const osg::Vec3d& vOSG) const
		{
			return SGMVector3(vOSG.x(), vOSG.y(), vOSG.z());
		}

		// ����
	private:

		SGMKernelData*						m_pKernelData = nullptr;		//!< �ں�����
		SGMConfigData*						m_pConfigData = nullptr;		//!< ��������
		CGMBaseManipulator*					m_pManipulator = nullptr;		//!< ����������

		bool								m_bInit = false;                //!< ��ʼ����־
		bool								m_bRendering = true;            //!< �Ƿ���Ⱦ
		double								m_dTimeLastFrame = 0.0;         //!< ��һ֡ʱ��
		float								m_fDeltaStep = 0.0f;            //!< ��λs
		float								m_fConstantStep = 0.1f;         //!< �ȼ�����µ�ʱ��,��λs

		CGMModel*							m_pModel = nullptr;				//!< ģ��ģ��
		CGMCharacter*						m_pCharacter = nullptr;			//!< ��ɫģ��
		CGMAudio*							m_pAudio = nullptr;				//!< ��Ƶģ��
		CGMPost*							m_pPost = nullptr;				//!< ����ģ��

		osg::ref_ptr<osg::Texture2D>		m_pSceneTex = nullptr;			//!< ��������ɫͼ
		osg::ref_ptr<osg::Texture2D>		m_pBackgroundTex = nullptr;		//!< ������ɫͼ
		osg::ref_ptr<osg::Texture2D>		m_pForegroundTex = nullptr;		//!< ǰ����ɫͼ
	};
}	// GM