//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		StackedSoftRotElement.h
/// @brief		GMEngine - Stacked Soft  Rotate Axis Element
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.15
//////////////////////////////////////////////////////////////////////////

#include "StackedSoftRotElement.h"

using namespace GM;

/*************************************************************************
Global Constants
*************************************************************************/

#define  MIN_ANGLE_ACCELERATION				(0.01)		// 引起抖动的最小的角加速度

StackedSoftRotElement::StackedSoftRotElement(){}

StackedSoftRotElement::StackedSoftRotElement(const StackedSoftRotElement& rhs, const osg::CopyOp& co)
	: StackedRotateAxisElement(rhs, co){}

StackedSoftRotElement::StackedSoftRotElement(const std::string& name, const osg::Vec3& axis, double angle)
	: osgAnimation::StackedRotateAxisElement(name, axis, angle){}

StackedSoftRotElement::StackedSoftRotElement(const osg::Vec3& axis, double angle)
	: osgAnimation::StackedRotateAxisElement(axis, angle)
{
	setName("quaternion");
}

StackedSoftRotElement::StackedSoftRotElement(const std::string& name, const osg::Vec3& axis, double angle, double fElastic)
	: osgAnimation::StackedRotateAxisElement(name, axis, angle)
{
	_fElastic = fElastic;
}

void StackedSoftRotElement::update(float frame)
{
	StackedRotateAxisElement::update(frame);
	if (0 == _fElastic) return;

	_fRigAngle = _angle;
	double _fRigAngleVelocity = _fRigAngle - _fLastRigAngle;

	double fRigAngleAcce = _fLastRigAngleVelocity - _fRigAngleVelocity;
	double fElasticAcce = -_fShakeValue / _fElastic;

	_vShakeVelocity += fElasticAcce + fRigAngleAcce;
	_fShakeValue += _vShakeVelocity;
	_angle += _fShakeValue;

	_fShakeValue *= 0.95;

	_fLastRigAngle = _fRigAngle;
	_fLastRigAngleVelocity = _fRigAngleVelocity;
}
