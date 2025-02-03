//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMCommonUniform.cpp
/// @brief		Galaxy-Music Engine - Common Uniform
/// @version	1.0
/// @author		LiuTao
/// @date		2022.08.21
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommonUniform.h"
#include <osg/Timer>

using namespace GM;

/*************************************************************************
 Macro Defines
*************************************************************************/

/*************************************************************************
CGMCommonUniform Methods
*************************************************************************/

template<> CGMCommonUniform* CGMSingleton<CGMCommonUniform>::msSingleton = nullptr;

/** @brief ��ȡ���� */
CGMCommonUniform& CGMCommonUniform::getSingleton(void)
{
	if (!msSingleton)
		msSingleton = GM_NEW(CGMCommonUniform);
	assert(msSingleton);
	return (*msSingleton);
}

CGMCommonUniform::CGMCommonUniform(): m_pKernelData(nullptr),
	m_vScreenSizeUniform(new osg::Uniform("screenSize", osg::Vec3f(1920.0f, 1080.0f, 0.5f))),
	m_fTimeUniform(new osg::Uniform("times", 0.0f)),
	m_mDeltaVPMatrixUniform(new osg::Uniform("deltaViewProjMatrix", osg::Matrixf())),
	m_mMainInvProjUniform(new osg::Uniform("invProjMatrix", osg::Matrixf())),
	m_vEyeFrontDirUniform(new osg::Uniform("eyeFrontDir", osg::Vec3f(0.0f, 0.0f, -1.0f))),
	m_vEyeRightDirUniform(new osg::Uniform("eyeRightDir", osg::Vec3f(1.0f, 0.0f, 0.0f))),
	m_vEyeUpDirUniform(new osg::Uniform("eyeUpDir", osg::Vec3f(0.0f, 1.0f, 0.0f))),
	m_vViewUpUniform(new osg::Uniform("viewUp", osg::Vec3f(0.0f, 1.0f, 0.0f))),
	m_fRenderingTime(0.0)
{
}

CGMCommonUniform::~CGMCommonUniform()
{
}

void CGMCommonUniform::Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData)
{
	m_pKernelData = pKernelData;

	int iScreenWidth = pConfigData->iScreenWidth;
	int iScreenHeight = pConfigData->iScreenHeight;

	m_vScreenSizeUniform->set(osg::Vec3f(iScreenWidth, iScreenHeight, 0.5f));
}

void CGMCommonUniform::Release()
{
	GM_DELETE(msSingleton);
}

void CGMCommonUniform::Update(double dDeltaTime)
{
	m_fRenderingTime += dDeltaTime;
	m_fTimeUniform->set(float(m_fRenderingTime));
}

void CGMCommonUniform::UpdatePost(double dDeltaTime)
{
	osg::Matrixd mViewMatrix = GM_View->getCamera()->getViewMatrix();
	osg::Vec3d vEye, vCenter, vUp;
	GM_View->getCamera()->getViewMatrixAsLookAt(vEye, vCenter, vUp);

	// �������view�ռ��µ�Up��������shader��
	osg::Vec3d vWorldSky = vEye;
	double fEye2Center = vWorldSky.normalize();
	osg::Vec4d v4ViewUp = mViewMatrix.preMult(osg::Vec4d(vWorldSky, 0.0));
	osg::Vec3d vViewUp = osg::Vec3d(v4ViewUp.x(), v4ViewUp.y(), v4ViewUp.z());
	vViewUp.normalize();
	m_vViewUpUniform->set(osg::Vec3f(vViewUp));
}

void CGMCommonUniform::ResizeScreen(const int width, const int height)
{
	m_vScreenSizeUniform->set(osg::Vec3f(width, height, 0.5f));
}