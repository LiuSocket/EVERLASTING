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

#include "StackedSoftTransElement.h"

using namespace GM;

StackedSoftTransElement::StackedSoftTransElement()
{
	Init();
}

StackedSoftTransElement::StackedSoftTransElement(const StackedSoftTransElement& rhs, const osg::CopyOp& co)
	: StackedTranslateElement(rhs, co)
{
	Init();
}

StackedSoftTransElement::StackedSoftTransElement(const std::string& name, const osg::Vec3& translate)
	: osgAnimation::StackedTranslateElement(name, translate)
{
	Init();
}

StackedSoftTransElement::StackedSoftTransElement(const osg::Vec3& translate)
	: osgAnimation::StackedTranslateElement(translate)
{
	setName("translate");
	Init();
}

StackedSoftTransElement::StackedSoftTransElement(const std::string& name, const osg::Vec3& translate,
	const osg::Vec3& vSoftRange, const osg::Vec3& vSoftCenter)
	: osgAnimation::StackedTranslateElement(name, translate)
{
	_vSoftVelocityRange = vSoftRange / (osg::PI * 2);
	_vSoftCenter = vSoftCenter;
	Init();
}

void StackedSoftTransElement::Init()
{
	if (_bInit) return;

	// 根据每个骨骼的位置生成不同的随机种子
	int seed = 10000 * std::abs(_translate.x() + _translate.y() + _translate.z());
	m_iRandom.seed(seed);
	std::uniform_int_distribution<> iPseudoNoise(0, 9999);
	// 生成随机相位
	_vSoftPhase = osg::Vec3(
		iPseudoNoise(m_iRandom) * 1e-3f,
		iPseudoNoise(m_iRandom) * 1e-3f,
		iPseudoNoise(m_iRandom) * 1e-3f);

	_bInit = true;
}

void StackedSoftTransElement::update(float frame)
{
	StackedTranslateElement::update(frame);
	if (0 == _vSoftVelocityRange.length2()) return;

	_vRigTranslate = _translate;
	osg::Vec3 _vDeltaPos = _vRigTranslate - _vLastRigTranslate;
	if ((_vDeltaPos.length2() - _vLastDeltaPos.length2()) > 0.01f)
	{
		_vSoftVelocityMax = _vSoftVelocityRange;
	}
	_vLastRigTranslate = _vRigTranslate;
	_vLastDeltaPos = _vDeltaPos;

	_vSoftVelocity = osg::Vec3(
		_vSoftVelocityMax.x() * sin(frame * 0.5 + _vSoftPhase.x()),
		_vSoftVelocityMax.y() * sin(frame * 1.37 + _vSoftPhase.y()),
		_vSoftVelocityMax.z() * sin(frame * 1.31 + _vSoftPhase.z()));
	_vSoftTrans += _vSoftVelocity;
	_translate += _vSoftTrans + _vSoftCenter;

	_vSoftVelocityMax *= 0.98;
	_vSoftTrans *= 0.98;
}
