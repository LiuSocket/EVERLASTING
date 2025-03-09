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
		StackedSoftRotElement(const std::string& name, const osg::Vec3& axis, double angle, double fShakeRange);

		void Init();
		void update(float frame = 0.0);

	protected:
		bool    _bInit = false;
		std::default_random_engine  m_iRandom;						//!< Ëæ»úÖµ
		double _fShakeVelocityRange = 0.0;
		double _fShakePhase = 0.0;

		double _vShakeVelocity = 0.0;
		double _vShakeVelocityMax = 0.0;

		double _fShake = 0.0;
		double _fRigAngle = 0.0;
		double _fLastRigAngle = 0.0;
		double _fLastDeltaAngle = 0.0;
	};
} // namespace GM
