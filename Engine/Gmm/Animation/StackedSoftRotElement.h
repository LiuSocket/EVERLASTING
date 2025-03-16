//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		StackedSoftRotElement.h
/// @brief		GMEngine - Stacked Soft Rotate Axis Element
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.15
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <random>
#include <osgAnimation/StackedRotateAxisElement>

namespace GM
{
	class StackedSoftRotElement : public osgAnimation::StackedRotateAxisElement
	{
	public:
		StackedSoftRotElement();
		StackedSoftRotElement(const StackedSoftRotElement&, const osg::CopyOp&);
		StackedSoftRotElement(const std::string& name, const osg::Vec3& axis, double angle);
		StackedSoftRotElement(const osg::Vec3& axis, double angle);
		StackedSoftRotElement(const std::string& name, const osg::Vec3& axis, double angle, double fElastic);

		void update(float frame = 0.0);

	protected:

		double _fElastic = 0.0;

		double _vShakeVelocity = 0.0;
		double _fShakeValue = 0.0;

		double _fRigAngle = 0.0;
		double _fLastRigAngle = 0.0;
		double _fLastRigAngleVelocity = 0.0;
	};
} // namespace GM
