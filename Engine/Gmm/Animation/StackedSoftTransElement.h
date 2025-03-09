//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		StackedSoftTransElement.h
/// @brief		GMEngine - Stacked Soft Translate Element
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.15
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <random>
#include <osgAnimation/StackedTranslateElement>

namespace GM
{
	class StackedSoftTransElement : public osgAnimation::StackedTranslateElement
	{
	public:
		StackedSoftTransElement();
		StackedSoftTransElement(const StackedSoftTransElement&, const osg::CopyOp&);
		StackedSoftTransElement(const std::string& name, const osg::Vec3& translate = osg::Vec3(0,0,0));
		StackedSoftTransElement(const osg::Vec3& translate);
		StackedSoftTransElement(const std::string& name, const osg::Vec3& translate,
			const osg::Vec3& vSoftRange, const osg::Vec3& vSoftCenter);

		void Init();
		void update(float frame = 0.0);

	protected:
		bool    _bInit = false;
		std::default_random_engine  m_iRandom;						//!< Ëæ»úÖµ
		osg::Vec3 _vSoftVelocityRange = osg::Vec3(0.0f, 0.0f, 0.0f);
		osg::Vec3 _vSoftCenter = osg::Vec3(0.0f, 0.0f, 0.0f);
		osg::Vec3 _vSoftPhase = osg::Vec3(0.0f, 0.0f, 0.0f);

		osg::Vec3 _vSoftVelocity = osg::Vec3(0.0f, 0.0f, 0.0f);
		osg::Vec3 _vSoftVelocityMax = osg::Vec3(0.0f, 0.0f, 0.0f);

		osg::Vec3 _vSoftTrans = osg::Vec3(0.0f, 0.0f, 0.0f);
		osg::Vec3 _vRigTranslate = osg::Vec3(0.0f, 0.0f, 0.0f);
		osg::Vec3 _vLastRigTranslate = osg::Vec3(0.0f, 0.0f, 0.0f);
		osg::Vec3 _vLastDeltaPos = osg::Vec3(0.0f, 0.0f, 0.0f);
	};
} // namespace GM
