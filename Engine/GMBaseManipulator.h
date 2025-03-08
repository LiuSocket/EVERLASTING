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

		//所有操作在这里响应
		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	protected:

		virtual ~CGMBaseManipulator();

	private:
		/**
		* @brief 间隔更新
		* @param updateStepTime 两次间隔更新的时间差，单位s
		*/
		void _InnerUpdate(const float updateStepTime);

		/**
		* @brief 映射函数
		* @param fX:					映射前的值
		* @param fMin, fMax:			映射前的范围
		* @param fMinNew, fMaxNew:		映射后的范围
		* @return double:				映射后的值
		*/
		inline double _Remap(const double fX,
			const double fMin, const double fMax,
			const double fMinNew, const double fMaxNew)
		{
			double fY = (osg::clampBetween(fX, fMin, fMax) - fMin) / (fMax - fMin);
			return fMinNew + fY * (fMaxNew - fMinNew);
		}

		/**
		* @brief 平滑过渡函数
		* @param fMin, fMax:			范围
		* @param fX:					映射前的值
		* @return double:				映射后的值
		*/
		inline double _Smoothstep(const double fMin, const double fMax, const double fX)
		{
			double y = osg::clampBetween((fX - fMin) / (fMax - fMin), 0.0, 1.0);
			return y * y * (3 - 2 * y);
		}

		/**
		* @brief 混合函数,参考 glsl 中的 mix(a,b,x)
		* @param fMin, fMax:			范围
		* @param fX:					混合系数
		* @return double/Vec3d:			混合后的值
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
		* @brief SGMVector3 转 osg::Vec3d
		* @param vGM:				输入的GM向量
		* @return osg::Vec3d:		输出的osg向量
		*/
		inline osg::Vec3d _GM2OSG(const SGMVector3& vGM) const
		{
			return osg::Vec3d(vGM.x, vGM.y, vGM.z);
		}
		/**
		* @brief SGMVector4 转 osg::Vec4d
		* @param vGM:				输入的GM向量
		* @return osg::Vec4d:		输出的osg向量
		*/
		inline osg::Vec4d _GM2OSG(const SGMVector4& vGM) const
		{
			return osg::Vec4d(vGM.x, vGM.y, vGM.z, vGM.w);
		}
		/**
		* @brief SGMVector3 转 osg::Vec3d
		* @param vGM:				输入的GM向量
		* @return osg::Vec3d:		输出的osg向量
		*/
		inline SGMVector3 _OSG2GM(const osg::Vec3d& vOSG) const
		{
			return SGMVector3(vOSG.x(), vOSG.y(), vOSG.z());
		}
		/**
		* @brief SGMVector4 转 osg::Vec4d
		* @param vGM:				输入的GM向量
		* @return osg::Vec4d:		输出的osg向量
		*/
		inline SGMVector4 _OSG2GM(const osg::Vec4d& vOSG) const
		{
			return SGMVector4(vOSG.x(), vOSG.y(), vOSG.z(), vOSG.w());
		}

	private:
		osg::Vec3d				m_vTranslate;               // 平移  
		osg::Quat				m_qRotate;					// 旋转
		float					m_fDeltaStep;				// 单位：s
		float					m_fConstantStep;			// 等间隔更新的时间,单位s
		float					m_fCursorStillTime;			// 鼠标静止的持续时间,单位s
		double					m_fTimeLastFrame;			// 上一帧时间，单位：s

		osg::Vec3d				m_vEyeDeltaMove;				// 相机平移矢量
		osg::Vec3d				m_vEyeUp;						// 相机上方向
		osg::Vec3d				m_vEyeFront;					// 相机前方向
		osg::Vec3d				m_vEyeLeft;						// 相机左方向
		osg::Vec2f				m_vMousePushScreenPos;			// 鼠标点击的屏幕XY坐标
		osg::Vec2f				m_vMouseLastScreenPos;			// 上一帧鼠标点击的屏幕XY坐标
		osg::Vec2f				m_vMouseDragLastScreenPos;		// 拖拽时，上一帧鼠标的屏幕XY坐标
		bool					m_bDoubleLeftClick = false;		// 是否左键双击
	};

}	// GM
