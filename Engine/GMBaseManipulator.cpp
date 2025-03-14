//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMBaseManipulator.cpp
/// @brief		Galaxy-Music Engine - GMBaseManipulator
/// @version	1.0
/// @author		LiuTao
/// @date		2024.10.02
//////////////////////////////////////////////////////////////////////////
#include "GMBaseManipulator.h"
#include "GMEngine.h"
#include "GMSystemManager.h"

#include <iostream>

using namespace GM;

/*************************************************************************
 Macro Defines
*************************************************************************/

/*************************************************************************
CGMBaseManipulator Methods
*************************************************************************/

CGMBaseManipulator::CGMBaseManipulator()
	: osgGA::CameraManipulator(), m_vTranslate(osg::Vec3d(0.0, -75.0, 8.5)), // 单位：厘米
	m_qRotate(osg::Quat(0, osg::Vec3d(1, 0, 0), 0, osg::Vec3d(0, 1, 0), 0, osg::Vec3d(0, 0, 1))),
	m_fDeltaStep(0.0f), m_fConstantStep(0.5f), m_fCursorStillTime(0.0f),
	m_fTimeLastFrame(0.0), m_vEyeDeltaMove(0, 0, 0), m_vEyeUp(0, 0, 1), m_vEyeFront(0, 1, 0), m_vEyeLeft(-1,0,0),
	m_vMousePushScreenPos(0.0f, 0.0f), m_vMouseLastScreenPos(0.0f,0.0f),
	m_vMouseDragLastScreenPos(0.0f, 0.0f)
{
	osg::Matrix mCameraRotate;
	mCameraRotate.makeLookAt(osg::Vec3d(0, 0, 0), m_vEyeFront, m_vEyeUp);
	m_qRotate.set(mCameraRotate);
}

void
CGMBaseManipulator::setByMatrix(const osg::Matrixd& matrix)
{
	m_vTranslate = matrix.getTrans();
	m_qRotate = matrix.getRotate();
}

osg::Matrixd
CGMBaseManipulator::getMatrix() const
{
	osg::Matrix mat;
	mat.setRotate(m_qRotate);//先旋转
	mat.postMultTranslate(m_vTranslate);//后平移
	return mat;
}

osg::Matrixd
CGMBaseManipulator::getInverseMatrix() const
{
	osg::Matrix mat;
	mat.setRotate(-m_qRotate);//后旋转
	mat.preMultTranslate(-m_vTranslate);//先平移
	return mat;
}

bool
CGMBaseManipulator::handle(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & aa)
{
	switch (ea.getEventType())
	{
	case (osgGA::GUIEventAdapter::FRAME):
	{
		double timeCurrFrame = osg::Timer::instance()->time_s();
		double fDeltaTime = timeCurrFrame - m_fTimeLastFrame;

		float fInnerDeltaTime = fDeltaTime;
		fInnerDeltaTime += m_fDeltaStep;
		while (fInnerDeltaTime >= m_fConstantStep)
		{
			_InnerUpdate(m_fConstantStep);
			fInnerDeltaTime -= m_fConstantStep;
		}
		m_fDeltaStep = fInnerDeltaTime;

		m_fTimeLastFrame = timeCurrFrame;
		m_vMouseLastScreenPos = osg::Vec2f(ea.getX(), ea.getY());
	}
	break;
	case (osgGA::GUIEventAdapter::PUSH):
	{
		m_vMousePushScreenPos = osg::Vec2f(ea.getX(), ea.getY());

		switch (ea.getButton())
		{
		case (osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON):
		{
		}
		break;
		case (osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON):
		{

		}
		break;
		case (osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON):
		{

		}
		break;
		default:
			break;
		}

		m_vMouseDragLastScreenPos = m_vMousePushScreenPos;
	}
	break;
	case (osgGA::GUIEventAdapter::RELEASE):
	{
		switch (ea.getButton())
		{
		case (osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON):
		{
		}
		break;
		case (osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON):
		{
		}
		break;
		case (osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON):
		{
		}
		break;
		default:
			break;
		}
		m_bDoubleLeftClick = false;
		GM_ENGINE.SetLookTargetVisible(false);
	}
	break;
	case (osgGA::GUIEventAdapter::DRAG):
	{
		float fDeltaX = ea.getX() - m_vMouseDragLastScreenPos.x();
		float fDeltaY = ea.getY() - m_vMouseDragLastScreenPos.y();

		// 如果左键拖拽，则强迫让角色注视，但时间久了角色会累或者无聊
		GM_ENGINE.SetLookTargetVisible(true);
		GM_ENGINE.SetLookTargetPos(osg::Vec2f(ea.getX(), ea.getY()));
		
		m_vMouseDragLastScreenPos = osg::Vec2f(ea.getX(), ea.getY());
	}
	break;
	case (osgGA::GUIEventAdapter::SCROLL):
	{

	}
	break;
	case (osgGA::GUIEventAdapter::KEYDOWN):
	{

		switch (ea.getKey())
		{
		case 'A':
		case 'a':
		{
		}
		break;
		case 'D':
		case 'd':
		{
		}
		break;
		case 'E':
		case 'e':
		{
		}
		break;
		case 'Q':
		case 'q':
		{
		}
		break;
		case 'S':
		case 's':
		{
		}
		break;
		case 'W':
		case 'w':
		{
		}
		break;
		case 'X':
		case 'x':
		{
		}
		break;
		case osgGA::GUIEventAdapter::KEY_Space:
		{
		}
		break;
		case osgGA::GUIEventAdapter::KEY_Return:
		{
		}
		break;
		case osgGA::GUIEventAdapter::KEY_F2:
		{
		}
		break;
		case osgGA::GUIEventAdapter::KEY_F3:
		{
		}
		break;
		case osgGA::GUIEventAdapter::KEY_F4:
		{
		}
		break;
		case osgGA::GUIEventAdapter::KEY_F5:
		{
			GM_ENGINE.Load();
		}
		break;

		case osgGA::GUIEventAdapter::KEY_Shift_L:
		{
		}
		break;
		case osgGA::GUIEventAdapter::KEY_Control_L:
		{
		}
		break;
		case 19:
		{
			// ctrl + s
			GM_ENGINE.Save();
		}
		break;
		default:
			break;
		}

		GM_SYSTEM_MANAGER.GMKeyDown(EGMKeyCode(ea.getKey()));
	}
	break;
	case (osgGA::GUIEventAdapter::KEYUP):
	{
		switch (ea.getKey())
		{
		case 'A':
		case 'a':
		{
		}
		break;
		case 'D':
		case 'd':
		{
		}
		break;
		case 'E':
		case 'e':
		{
		}
		break;
		case 'Q':
		case 'q':
		{
		}
		break;
		case 'S':
		case 's':
		{
		}
		break;
		case 'W':
		case 'w':
		{
		}
		break;
		case 'X':
		case 'x':
		{
		}
		break;
		case osgGA::GUIEventAdapter::KEY_Up:
		case osgGA::GUIEventAdapter::KEY_Down:
		{
		}
		break;
		case osgGA::GUIEventAdapter::KEY_Space:
		{
		}
		break;
		case osgGA::GUIEventAdapter::KEY_Shift_L:
		{
		}
		break;
		case osgGA::GUIEventAdapter::KEY_Control_L:
		{
		}
		break;
		default:
			break;
		}

		GM_SYSTEM_MANAGER.GMKeyUp(EGMKeyCode(ea.getKey()));
	}
	break;
	case (osgGA::GUIEventAdapter::DOUBLECLICK):
	{
		if (osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON == ea.getButtonMask())
		{
			m_bDoubleLeftClick = true;
		}
	}
	break;
	default:
		break;
	}

	return false;
}

CGMBaseManipulator::~CGMBaseManipulator()
{
}

void CGMBaseManipulator::_InnerUpdate(const float updateStepTime)
{
}