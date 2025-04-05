//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMAudio.h
/// @brief		Galaxy-Music Engine - GMAudio
/// @version	1.0
/// @author		LiuTao
/// @date		2021.05.29
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommon.h"
#include "bass.h"
#include <osg/Vec2f>

namespace GM
{
	/*************************************************************************
	Enums
	*************************************************************************/

	// ��Ƶ��������ö��
	enum EGMA_COMMAND
	{
		EGMA_CMD_OPEN,				// ����Ƶ
		EGMA_CMD_PLAY,				// ������Ƶ
		EGMA_CMD_CLOSE,				// �ر���Ƶ
		EGMA_CMD_PAUSE,				// ��ͣ��Ƶ
		EGMA_CMD_STOP				// ֹͣ��Ƶ
	};

	// ��ǰ����״̬
	enum EGMA_STATE
	{
		EGMA_STA_MUTE,				// �Ѿ���
		EGMA_STA_PLAY,				// ���ڲ���
		EGMA_STA_PAUSE,				// ����ͣ
	};

	/*************************************************************************
	Structs
	*************************************************************************/

	/*************************************************************************
	Class
	*************************************************************************/

	/*!
	*  @class CGMAudio
	*  @brief Galaxy-Music CGMAudio
	*/
	class CGMAudio
	{
		// ����
	public:
		/** @brief ���� */
		CGMAudio();

		/** @brief ���� */
		~CGMAudio();

		/** @brief ��ʼ�� */
		bool Init(SGMConfigData* pConfigData);
		/** @brief ���� */
		bool Update(double dDeltaTime);

		/**
		* Welcome
		* @brief ��������ӭЧ����
		* @brief ÿ�ο��������Engine���ô˺�����ʵ�֡���ӭ���ܡ�
		*/
		void Welcome();

		/**
		* @brief ����ӭЧ�����Ƿ����
		* @return bool �����򷵻�true������false
		*/
		inline bool IsWelcomeFinished() const { return m_bWelcomeEnd;}

		/**
		* @brief �����ļ��������õ�ǰ��Ƶ
		* @param strAudioFile:	��ǰ��Ƶ�ļ����ƣ����磺xxx.mp3
		* @return bool ���óɹ�����true��ʧ�ܻ����ļ�δ�ҵ�����false
		*/
		bool SetCurrentAudio(const std::wstring& strAudioFile);

		/**
		* @brief ��ȡ��ǰ��Ƶ�ļ�����
		* @return std::wstring ��ǰ��Ƶ�ļ����ƣ����磺xxx.mp3,����L""
		*/
		inline std::wstring GetCurrentAudio() const{ return m_strCurrentFile; }

		/**
		* @brief ��Ƶ���ſ���
		* @param command:	ָ������ö��
		*/
		void AudioControl(EGMA_COMMAND command);

		/**
		* @brief �ж���Ƶ�Ƿ񲥷����
		* @return bool ��Ϸ���true��δ��Ϸ���false
		*/
		bool IsAudioOver() const;

		/**
		* @brief ����������ע�⣬��˲ʱ���ֵ����һ�����
		* @param fLevel ���� [0.0f,1.0f]
		* @return bool �Ϸ�����true�����Ϸ���ʧ�ܷ���false
		*/
		bool SetVolume(float fLevel);
		/**
		* @brief ��ȡ������ע�⣬��˲ʱ���ֵ����һ�����
		* @return float ���� [0.0f,1.0f]
		*/
		inline float GetVolume() const { return m_fVolume;}

		/**
		* @brief ��ȡ��ǰ֡���ֵ
		* @return float ��ǰ֡��� [0.0f,1.0f]
		*/
		float GetLevel() const;

		/**
		* @brief ��ȡ��ǰ������Ƶ��ʱ������λ��ms
		* @return int ��Ƶ��ʱ������λ��ms
		*/
		inline int GetAudioDuration() const { return m_iAudioDuration;}

		/**
		* @brief ������Ƶ�Ĳ���λ�ã���λ��ms
		* @param iTime: ��Ƶ�Ĳ���λ��
		* @return bool �ɹ�true�� ʧ��false
		*/
		bool SetAudioCurrentTime(int iTime);
		/**
		* @brief ��ȡ��ǰ��Ƶ�Ĳ���λ�ã���λ��ms
		* @return int ��Ƶ����λ�ã���λ��ms
		*/
		inline int GetAudioCurrentTime() const { return m_iAudioCurrentTime;}

	private:

		/**
		* @brief �������
		* @param updateStep ���μ�����µ�ʱ����λs
		*/
		void _InnerUpdate(float updateStep);

		/** @brief ��ʼ��BASS��Ƶ�� */
		inline void _InitBASS()
		{
			//��ʼ��BASS��Ƶ��
			BASS_Init(
				-1,//Ĭ���豸
				44100,//���������44100������ֵ��
				BASS_DEVICE_CPSPEAKERS,//�źţ�BASS_DEVICE_CPSPEAKERS 
				NULL,//���򴰿�,0���ڿ���̨����
				nullptr//���ʶ��,0ʹ��Ĭ��ֵ
			);
		}

		/**
		* @brief ��ȡ��ǰ���ŵ���λ�ã�ʱ�����꣩
		* @return int ��ǰ��Ƶ����λ�õ�ʱ�����꣬��λ��ms
		*/
		int _GetAudioCurrentTime() const;

		/**
		* @brief ���ÿ�ʼ���ŵ�ʱ������
		* @param time ʱ�����꣬��λms
		* @return bool �ɹ�true�� ʧ��false
		*/
		bool _SeekTo(int iTime);

		/** @brief Ϊ����ӭЧ������׼������ */
		void _PreWelcome();

		// ����
	private:
		SGMConfigData*						m_pConfigData = nullptr;				//!< ��������
		std::string							m_strCoreAudioPath = "Audios/";			//!< ������Ƶ���·��
		std::wstring						m_strAudioPath = L"Music/";				//!< ���ִ��·��
		std::wstring						m_strCurrentFile = L"";					//!< ���ڲ��ŵ��ļ���,XXX.mp3
		HSTREAM								m_streamAudio = 0;
		EGMA_STATE							m_eAudioState = EGMA_STA_MUTE;			//!< ��ǰ����״̬
		int									m_iAudioLastTime = 0;					//!< ��һ֡��Ƶʱ������,��λms
		int									m_iAudioCurrentTime = 0;				//!< ��ǰ֡��Ƶʱ������,��λms
		int									m_iAudioDuration = 0;					//!< ��ǰ���ŵ���Ƶʱ��,��λms

		bool								m_bWelcomeStart = false;				//!< ��ӭ���ܿ�ʼ��־
		bool								m_bWelcomeEnd = false;					//!< ��ӭ���ܽ�����־
		int									m_iWelcomeDuration = 5000;				//!< ��ӭ��Ƶʱ��,��λms

		float								m_fVolume = 0.5f;						//!< �������
	};
}	// GM
