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

    if (0.0001f > _vSoftTrans.length2())
	{
        //_vSoftTrans = _vSoftLimit;
	}

    osg::Vec3 _vSoftOffset = osg::Vec3(
        _vSoftTrans.x() * std::sin(t * 10.0 + _vSoftPhase.x()) * 0.5 + _vSoftLimit.x()*0.5,
        _vSoftTrans.y() * std::sin(t * 10.49 + _vSoftPhase.y()),
        _vSoftTrans.z() * std::sin(t * 10.31 + _vSoftPhase.z()));
    _translate += _vSoftOffset;

    _vSoftTrans *= 0.99f;
}
