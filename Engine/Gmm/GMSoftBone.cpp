//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMSoftBone.cpp
/// @brief		GMEngine - Soft Bone Module
/// @version	1.0
/// @author		LiuTao
/// @date		2025.02.09
//////////////////////////////////////////////////////////////////////////
#include "GMSoftBone.h"
#include "UpdateSoftBone.h"

using namespace GM;

CGMSoftBone::CGMSoftBone(const CGMSoftBone& b, const osg::CopyOp& copyop) : Bone(b,copyop)
{
}

CGMSoftBone::CGMSoftBone(const std::string& name) : Bone(name)
{
}


void CGMSoftBone::setDefaultUpdateCallback(const std::string& name)
{
    std::string cbName = name;
    if (cbName.empty())
        cbName = getName();
    setUpdateCallback(new UpdateSoftBone(cbName));
}

CGMSoftBone* CGMSoftBone::getBoneParent()
{
    if (getParents().empty())
        return 0;
    osg::Node::ParentList parents = getParents();
    for (osg::Node::ParentList::iterator it = parents.begin(); it != parents.end(); ++it)
    {
        CGMSoftBone* pb = dynamic_cast<CGMSoftBone*>(*it);
        if (pb)
            return pb;
    }
    return 0;
}
const CGMSoftBone* CGMSoftBone::getBoneParent() const
{
    if (getParents().empty())
        return 0;
    const osg::Node::ParentList& parents = getParents();
    for (osg::Node::ParentList::const_iterator it = parents.begin(); it != parents.end(); ++it)
    {
        const CGMSoftBone* pb = dynamic_cast<const CGMSoftBone*>(*it);
        if (pb)
            return pb;
    }
    return 0;
}