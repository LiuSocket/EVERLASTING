//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMBaseManipulator.h
/// @brief		Galaxy-Music Engine - GMBaseManipulator
/// @version	1.0
/// @author		LiuTao
/// @date		2024.10.02
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommon.h"
#include <osgGA/CameraManipulator>

namespace GM
{
	/*************************************************************************
	 Global Constants
	*************************************************************************/

	/*************************************************************************
	 Enums
	*************************************************************************/

	/*************************************************************************
	Structs
	*************************************************************************/

	/*************************************************************************
	 Class
	*************************************************************************/

	class CGMBaseManipulator : public osgGA::CameraManipulator
	{
	public:
		CGMBaseManipulator();

	public: // osgGA::CameraManipulator

		virtual const char* className() const { return "GMBaseManipulator"; }

		/** set the position of the matrix manipulator using a 4x4 Matrix.*/
		virtual void setByMatrix(const osg::Matrixd& matrix);

		/** set the position of the matrix manipulator using a 4x4 Matrix.*/
		virtual void setByInverseMatrix(const osg::Matrixd& matrix) { setByMatrix(osg::Matrixd::inverse(matrix)); }

		/** get the position of the manipulator as 4x4 Matrix.*/
		virtual osg::Matrixd getMatrix() const;

		/** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
		virtual osg::Matrixd getInverseMatrix() const;

		//���в�����������Ӧ
		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	protected:

		virtual ~CGMBaseManipulator();

	private:
		/**
		* @brief �������
		* @param updateStepTime ���μ�����µ�ʱ����λs
		*/
		void _InnerUpdate(const float updateStepTime);

		/**
		* @brief ӳ�亯��
		* @param fX:					ӳ��ǰ��ֵ
		* @param fMin, fMax:			ӳ��ǰ�ķ�Χ
		* @param fMinNew, fMaxNew:		ӳ���ķ�Χ
		* @return double:				ӳ����ֵ
		*/
		inline double _Remap(const double fX,
			const double fMin, const double fMax,
			const double fMinNew, const double fMaxNew)
		{
			double fY = (osg::clampBetween(fX, fMin, fMax) - fMin) / (fMax - fMin);
			return fMinNew + fY * (fMaxNew - fMinNew);
		}

		/**
		* @brief ƽ�����ɺ���
		* @param fMin, fMax:			��Χ
		* @param fX:					ӳ��ǰ��ֵ
		* @return double:				ӳ����ֵ
		*/
		inline double _Smoothstep(const double fMin, const double fMax, const double fX)
		{
			double y = osg::clampBetween((fX - fMin) / (fMax - fMin), 0.0, 1.0);
			return y * y * (3 - 2 * y);
		}

		/**
		* @brief ��Ϻ���,�ο� glsl �е� mix(a,b,x)
		* @param fMin, fMax:			��Χ
		* @param fX:					���ϵ��
		* @return double/Vec3d:			��Ϻ��ֵ
		*/
		inline double _Mix(const double fMin, const double fMax, const double fX)
		{
			return fMin*(1 - fX) + fMax*fX;
		}
		inline osg::Vec3d _Mix(const osg::Vec3d& vMin, const osg::Vec3d& vMax, const double fX)
		{
			return vMin * (1 - fX) + vMax * fX;
		}

		/**
		* @brief SGMVector3 ת osg::Vec3d
		* @param vGM:				�����GM����
		* @return osg::Vec3d:		�����osg����
		*/
		inline osg::Vec3d _GM2OSG(const SGMVector3& vGM) const
		{
			return osg::Vec3d(vGM.x, vGM.y, vGM.z);
		}
		/**
		* @brief SGMVector4 ת osg::Vec4d
		* @param vGM:				�����GM����
		* @return osg::Vec4d:		�����osg����
		*/
		inline osg::Vec4d _GM2OSG(const SGMVector4& vGM) const
		{
			return osg::Vec4d(vGM.x, vGM.y, vGM.z, vGM.w);
		}
		/**
		* @brief SGMVector3 ת osg::Vec3d
		* @param vGM:				�����GM����
		* @return osg::Vec3d:		�����osg����
		*/
		inline SGMVector3 _OSG2GM(const osg::Vec3d& vOSG) const
		{
			return SGMVector3(vOSG.x(), vOSG.y(), vOSG.z());
		}
		/**
		* @brief SGMVector4 ת osg::Vec4d
		* @param vGM:				�����GM����
		* @return osg::Vec4d:		�����osg����
		*/
		inline SGMVector4 _OSG2GM(const osg::Vec4d& vOSG) const
		{
			return SGMVector4(vOSG.x(), vOSG.y(), vOSG.z(), vOSG.w());
		}

	private:
		osg::Vec3d				m_vTranslate;               // ƽ��  
		osg::Quat				m_qRotate;					// ��ת
		float					m_fDeltaStep;				// ��λ��s
		float					m_fConstantStep;			// �ȼ�����µ�ʱ��,��λs
		float					m_fCursorStillTime;			// ��꾲ֹ�ĳ���ʱ��,��λs
		double					m_fTimeLastFrame;			// ��һ֡ʱ�䣬��λ��s

		osg::Vec3d				m_vEyeDeltaMove;				// ���ƽ��ʸ��
		osg::Vec3d				m_vEyeUp;						// ����Ϸ���
		osg::Vec3d				m_vEyeFront;					// ���ǰ����
		osg::Vec3d				m_vEyeLeft;						// �������
		osg::Vec2f				m_vMousePushScreenPos;			// ���������ĻXY����
		osg::Vec2f				m_vMouseLastScreenPos;			// ��һ֡���������ĻXY����
		osg::Vec2f				m_vMouseDragLastScreenPos;		// ��קʱ����һ֡������ĻXY����
		bool					m_bDoubleLeftClick = false;		// �Ƿ����˫��
	};

}	// GM
