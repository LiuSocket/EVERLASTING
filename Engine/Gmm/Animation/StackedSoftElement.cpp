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

StackedSoftElement::StackedSoftElement(const std::string& name, const osg::Vec3& translate)
    : osgAnimation::StackedTranslateElement(name,translate) {}

StackedSoftElement::StackedSoftElement(const osg::Vec3& translate)
    : osgAnimation::StackedTranslateElement(translate)
{
    osgAnimation::StackedTranslateElement::setName("translate");
}

StackedSoftElement::StackedSoftElement() {}
StackedSoftElement::StackedSoftElement(const StackedSoftElement& rhs, const osg::CopyOp& co)
    : StackedTranslateElement(rhs, co)
{
}

void StackedSoftElement::update(float t)
{
    StackedTranslateElement::update(t);

    //_translate += _vSoftTranslate * (std::sin(t*10) * 0.5 + 0.5);
}
