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

StackedSoftElement::StackedSoftElement(const std::string& name, const osg::Vec3& translate, const osg::Vec3& vLimit)
    : osgAnimation::StackedTranslateElement(name, translate)
{
    _vSoftLimit = vLimit;
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

    osg::Vec3 _vSoftOffset = osg::Vec3(
        _vSoftLimit.x() * (std::sin(t * 10 + _vSoftPhase.x()) * 0.1 + 1.8),
        _vSoftLimit.y() * std::sin(t * 19.5 + _vSoftPhase.y()),
        _vSoftLimit.z() * std::sin(t * 19.1 + _vSoftPhase.z()));
    _translate += _vSoftOffset;
}
