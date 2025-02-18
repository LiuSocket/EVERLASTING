//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		StackedSoftElement.h
/// @brief		GMEngine - Stacked Soft Element
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.15
//////////////////////////////////////////////////////////////////////////

#include "StackedSoftElement.h"

using namespace GM;

StackedSoftElement::StackedSoftElement()
{
    Init();
}

StackedSoftElement::StackedSoftElement(const StackedSoftElement& rhs, const osg::CopyOp& co)
    : StackedTranslateElement(rhs, co)
{
    Init();
}

StackedSoftElement::StackedSoftElement(const std::string& name, const osg::Vec3& translate)
    : osgAnimation::StackedTranslateElement(name, translate)
{
    Init();
}

StackedSoftElement::StackedSoftElement(const osg::Vec3& translate)
    : osgAnimation::StackedTranslateElement(translate)
{
    setName("translate");
    Init();
}

StackedSoftElement::StackedSoftElement(const std::string& name, const osg::Vec3& translate,
    const osg::Vec3& vSoftRange, const osg::Vec3& vSoftCenter)
    : osgAnimation::StackedTranslateElement(name, translate)
{
    _vSoftRange = vSoftRange;
    _vSoftCenter = vSoftCenter;
    Init();
}

void StackedSoftElement::Init()
{
    if (_bInit) return;

    // 根据每个骨骼的位置生成不同的随机种子
    m_iRandom.seed(10000 * std::abs(_translate.x() + _translate.y() + _translate.z()));
    std::uniform_int_distribution<> iPseudoNoise(0, 9999);
    // 生成随机相位
    _vSoftPhase = osg::Vec3(
        iPseudoNoise(m_iRandom) * 1e-3f,
        iPseudoNoise(m_iRandom) * 1e-3f,
        iPseudoNoise(m_iRandom) * 1e-3f);

    _bInit = true;
}

void StackedSoftElement::update(float t)
{
    StackedTranslateElement::update(t);
    _vRigTranslate = _translate;
    osg::Vec3 _vDeltaPos = _vRigTranslate - _vLastRigTranslate;
    if (_vDeltaPos.length2() > (_vLastDeltaPos.length2() + 1e-5))
    {
        _vSoftTrans = _vSoftRange;

		//OSG_WARN << "_vDeltaPos: "
		//	<< ((abs(_vDeltaPos.x()) > 1e-3f) ? _vDeltaPos.x() : 0.0f) << ", "
		//	<< ((abs(_vDeltaPos.y()) > 1e-3f) ? _vDeltaPos.y() : 0.0f) << ", "
		//	<< ((abs(_vDeltaPos.z()) > 1e-3f) ? _vDeltaPos.z() : 0.0f) << std::endl;
    }
    _vLastRigTranslate = _vRigTranslate;
    _vLastDeltaPos = _vDeltaPos;

    osg::Vec3 _vSoftOffset = osg::Vec3(
        _vSoftTrans.x() * sin(t * 30.0 + _vSoftPhase.x()) + _vSoftCenter.x(),
        _vSoftTrans.y() * sin(t * 81.0 + _vSoftPhase.y()) + _vSoftCenter.y(),
        _vSoftTrans.z() * sin(t * 77.0 + _vSoftPhase.z()) + _vSoftCenter.z());
    _translate += _vSoftOffset;

    _vSoftTrans *= 0.98f;
}
