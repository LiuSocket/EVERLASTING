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

StackedSoftRotElement::StackedSoftRotElement()
{
	Init();
}

StackedSoftRotElement::StackedSoftRotElement(const StackedSoftRotElement& rhs, const osg::CopyOp& co)
	: StackedRotateAxisElement(rhs, co)
{
	Init();
}

StackedSoftRotElement::StackedSoftRotElement(const std::string& name, const osg::Vec3& axis, double angle)
	: osgAnimation::StackedRotateAxisElement(name, axis, angle)
{
	Init();
}

StackedSoftRotElement::StackedSoftRotElement(const osg::Vec3& axis, double angle)
	: osgAnimation::StackedRotateAxisElement(axis, angle)
{
	setName("quaternion");
	Init();
}

StackedSoftRotElement::StackedSoftRotElement(const std::string& name,
	const osg::Vec3& axis, double angle, double fShakeRange)
	: osgAnimation::StackedRotateAxisElement(name, axis, angle)
{
	_fShakeVelocityRange = fShakeRange / (osg::PI * 2);
	Init();
}

void StackedSoftRotElement::Init()
{
	if (_bInit) return;

	// 根据每个骨骼的位置生成不同的随机种子
	int seed = 10000 * std::abs(_axis.x() + _axis.y() + _axis.z() + _angle);
	m_iRandom.seed(seed);
	std::uniform_int_distribution<> iPseudoNoise(0, 9999);
	// 生成随机相位
	_fShakePhase = iPseudoNoise(m_iRandom) * 1e-3f;

	_bInit = true;
}

void StackedSoftRotElement::update(float frame)
{
	StackedRotateAxisElement::update(frame);
	if (0 == _fShakeVelocityRange) return;

	_fRigAngle = _angle;
	double _fDeltaRigAngle = _fRigAngle - _fLastRigAngle;
	double fDDAngle = abs(_fDeltaRigAngle - _fLastDeltaAngle);
	if (fDDAngle > MIN_ANGLE_ACCELERATION)
	{
		_vShakeVelocityMax = fmin(2 * (fDDAngle - MIN_ANGLE_ACCELERATION), _fShakeVelocityRange);
	}
	_fLastRigAngle = _fRigAngle;
	_fLastDeltaAngle = _fDeltaRigAngle;

	_vShakeVelocity = _vShakeVelocityMax * sin(frame * 0.2 + _fShakePhase);
	_fShake += _vShakeVelocity;
	_angle += _fShake;

	_vShakeVelocityMax *= 0.96;
	_fShake *= 0.96;
}
