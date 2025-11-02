//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMVolumeBasic.h
/// @brief		Galaxy-Music Engine - GMEngine.cpp
/// @version	1.0
/// @author		LiuTao
/// @date		2020.12.10
//////////////////////////////////////////////////////////////////////////

#include "GMEngine.h"
#include "GMViewWidget.h"
#include "GMCommonUniform.h"
#include "GMBaseManipulator.h"
#include "GMXml.h"
#include "GMPost.h"
#include "GMTerrain.h"
#include "GMModel.h"
#include "GMCharacter.h"
#include "GMLight.h"
#include "GMAudio.h"
#include "Animation/GMAnimation.h"
#include "osgQt/GraphicsWindowQt.h"
#include <osgViewer/ViewerEventHandlers>
#include <QtCore/QTimer>

#include <iostream>

using namespace GM;

/*************************************************************************
Global Constants
*************************************************************************/
static const std::string g_strGMConfigFile = "Everlasting.cfg";	//!< 配置文件名

/*************************************************************************
 Macro Defines
*************************************************************************/

/*************************************************************************
 CGMEngine Methods
*************************************************************************/

class ResizeEventHandler : public osgGA::GUIEventHandler
{
public:
	ResizeEventHandler(CGMEngine* pEngine): _pEngine(pEngine){}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&)
	{
		if ((osgGA::GUIEventAdapter::RESIZE & ea.getEventType()) && _pEngine)
		{
			_pEngine->ResizeScreen(ea.getWindowWidth(), ea.getWindowHeight());
			return true;
		}
		return false;
	}
private:
	CGMEngine* _pEngine = nullptr;
};

template<> CGMEngine* CGMSingleton<CGMEngine>::msSingleton = nullptr;

/** @brief 获取单例 */
CGMEngine& CGMEngine::getSingleton(void)
{
	if (!msSingleton)
		msSingleton = GM_NEW(CGMEngine);
	assert(msSingleton);
	return (*msSingleton);
}

/** @brief 构造 */
CGMEngine::CGMEngine()
{
	Init();
}

/** @brief 析构 */
CGMEngine::~CGMEngine()
{
	GM_DELETE(msSingleton);
}

/** @brief 初始化 */
bool CGMEngine::Init()
{
	if (m_bInit) return true;

	//!< 配置数据
	_LoadConfig();

	//!< 内核数据
	m_pKernelData = new SGMKernelData();

	GM_Root = new osg::Group();
	GM_View = new osgViewer::View();
	GM_View->setSceneData(GM_Root);

	GM_LIGHT.Init(m_pKernelData, m_pConfigData);

	m_pSceneTex = new osg::Texture2D();
	m_pSceneTex->setTextureSize(m_pConfigData->iScreenWidth, m_pConfigData->iScreenHeight);
	m_pSceneTex->setInternalFormat(GL_RGBA8);
	m_pSceneTex->setSourceFormat(GL_RGBA);
	m_pSceneTex->setSourceType(GL_UNSIGNED_BYTE);
	m_pSceneTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	m_pSceneTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	m_pSceneTex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	m_pSceneTex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	m_pSceneTex->setBorderColor(osg::Vec4d(0, 0, 0, 0));
	m_pSceneTex->setDataVariance(osg::Object::DYNAMIC);
	m_pSceneTex->setResizeNonPowerOfTwoHint(false);

	// 初始化背景相关节点
	_InitBackground();
	// 初始化前景相关节点
	_InitForeground();

	m_pManipulator = new CGMBaseManipulator();
	m_pTerrain = new CGMTerrain();
	m_pModel = new CGMModel();
	m_pCharacter = new CGMCharacter();
	m_pAudio = new CGMAudio();
	m_pPost = new CGMPost();

	GM_UNIFORM.Init(m_pKernelData, m_pConfigData);
	m_pTerrain->Init(m_pKernelData, m_pConfigData);
	m_pModel->Init(m_pKernelData, m_pConfigData);
	m_pCharacter->Init(m_pKernelData, m_pConfigData);
	m_pAudio->Init(m_pConfigData);
	m_pPost->Init(m_pKernelData, m_pConfigData);

	GM_View->getCamera()->setCullMask(GM_MAIN_MASK);
	GM_View->setCameraManipulator(m_pManipulator);
	//状态信息
	osgViewer::StatsHandler* pStatsHandler = new osgViewer::StatsHandler;
	pStatsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
	GM_View->addEventHandler(pStatsHandler);

	m_bInit = true;
	m_pKernelData->bInited = m_bInit;

	GM_View->addEventHandler(new ResizeEventHandler(this));

	SLightData sMainLight;
	sMainLight.strName = "mainLight";
	sMainLight.eType = EGMLIGHT_SOURCE_DIRECTIONAL;
	sMainLight.vDir = osg::Vec4d(-1.0, 1.5, -1.0, 0.0);
	sMainLight.fLuminous = 2e3f;
	sMainLight.bShadow = true;
	GM_LIGHT.Add(sMainLight);

	// 加载背景模型
	SGMModelData sData = SGMModelData();
	sData.strName = "Bacdground";
	sData.strFilePath = "Background.FBX";
	sData.iEntRenderBin = 0;
	sData.eMaterial = EGM_MATERIAL_Background;
	sData.bCastShadow = false;
	m_pModel->Add(sData);

	// 加载角色模型
	SGMModelData sData1 = SGMModelData();
	sData1.strName = "MIGI";
	sData1.strFilePath = "MIGI.CIP";
	sData1.eMaterial = EGM_MATERIAL_Human;
	m_pModel->Add(sData1);

	m_pModel->SetAnimationEnable("MIGI", true);
	m_pCharacter->InitAnimation("MIGI", m_pModel->GetNode("MIGI"));

	std::vector<osg::ref_ptr<osg::Transform>> pEyeVector;
	m_pModel->GetEyeTransform(pEyeVector);
	m_pCharacter->InitEyeTransform(pEyeVector);

	return true;
}

/** @brief 释放 */
void CGMEngine::Release()
{
	setlocale(LC_ALL, "C");
	if (GM_Viewer.valid())
	{
		GM_Viewer->stopThreading();
		GM_Viewer = 0L;
	}

	GM_LIGHT.Release();
	GM_UNIFORM.Release();
	GM_ANIMATION.Release();

	GM_DELETE(m_pAudio);
	GM_DELETE(m_pCharacter);
	GM_DELETE(m_pTerrain);
	GM_DELETE(m_pModel);
	GM_DELETE(m_pPost);

	GM_DELETE(m_pConfigData);
	GM_DELETE(m_pKernelData);
	GM_DELETE(msSingleton);
}

/** @brief 更新 */
bool CGMEngine::Update()
{
	if (!m_bInit)
		return false;

	if (GM_Viewer->done())
		return true;

	if (GM_Viewer->checkNeedToDoFrame())
	{
		double timeCurrFrame = osg::Timer::instance()->time_s();
		double dDeltaTime = timeCurrFrame - m_dTimeLastFrame; //单位:秒
		m_dTimeLastFrame = timeCurrFrame;

		static double s_fConstantStep = 0.1;		//!< 等间隔更新的时间,单位s
		static double s_fDeltaStep = 0.0;			//!< 单位s
		if (s_fDeltaStep > s_fConstantStep)
		{
			if (m_bRendering)
				_InnerUpdate(s_fDeltaStep);
			s_fDeltaStep = 0.0;
		}
		s_fDeltaStep += dDeltaTime;

		m_pAudio->Update(dDeltaTime);
		if (m_bRendering)
		{
			GM_LIGHT.Update(dDeltaTime);

			GM_UNIFORM.Update(dDeltaTime);
			m_pPost->Update(dDeltaTime);
			m_pTerrain->Update(dDeltaTime);
			m_pModel->Update(dDeltaTime);
			m_pCharacter->Update(dDeltaTime);

			GM_Viewer->advance(USE_REFERENCE_TIME);
			GM_Viewer->eventTraversal();
			GM_Viewer->updateTraversal();

			// 在主相机改变位置后再更新
			_UpdateLater(dDeltaTime);

			GM_Viewer->renderingTraversals();
		}
	}
	return true;
}

/** @brief 加载 */
bool CGMEngine::Load()
{
	GM_LIGHT.Clear();

	m_pPost->Load();
	m_pTerrain->Load();
	m_pModel->Load();

	return true;
}

/** @brief 保存 */
bool CGMEngine::Save()
{
	m_pModel->Save();
	return false;
}

void CGMEngine::ResizeScreen(const int iW, const int iH)
{
	m_pConfigData->iScreenWidth = iW;
	m_pConfigData->iScreenHeight = iH;

	osg::ref_ptr<osg::Camera> pMainCam = GM_View->getCamera();
	if (pMainCam.valid())
	{
		double fovy, aspectRatio, zNear, zFar;
		pMainCam->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

		pMainCam->resize(iW, iH);
		double fAspetRatio = double(iW) / double(iH);
		pMainCam->setProjectionMatrixAsPerspective(fovy, fAspetRatio, zNear, zFar);
		pMainCam->dirtyAttachmentMap();
	}

	GM_UNIFORM.ResizeScreen(iW, iH);

	if (m_pKernelData->pBackgroundCam.valid())
		m_pKernelData->pBackgroundCam->resize(iW, iH);
	if (m_pKernelData->pForegroundCam.valid())
		m_pKernelData->pForegroundCam->resize(iW, iH);
	if (m_pPost)
		m_pPost->ResizeScreen(iW, iH);

	m_pTerrain->ResizeScreen(iW, iH);
	m_pModel->ResizeScreen(iW, iH);
}

void CGMEngine::SetLookTargetPos(const SGMVector2f& vTargetScreenPos)
{
	osg::Vec2f vTargetScreePos(vTargetScreenPos.x, vTargetScreenPos.y);

	double fovy, aspectRatio, zNear, zFar;
	GM_View->getCamera()->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
	osg::Vec3d vEye, vCenter, vUp;
	GM_View->getCamera()->getViewMatrixAsLookAt(vEye, vCenter, vUp);
	osg::Vec3d vFrontDir = vCenter - vEye;
	vFrontDir.normalize();

	// 假设目标在相机前方60cm处
	const double fLen = 60;
	double fHalfH = fLen * tan(osg::DegreesToRadians(fovy * 0.5));
	osg::Vec3d vTargetWorldPos = vEye + vFrontDir * fLen;
	vTargetWorldPos.x() += (vTargetScreePos.x() / m_pConfigData->iScreenWidth - 0.5) * 2 * fHalfH * aspectRatio;
	vTargetWorldPos.z() += (vTargetScreePos.y() / m_pConfigData->iScreenHeight-0.5) * 2 * fHalfH;
	m_pCharacter->SetLookTargetPos(vTargetWorldPos);
}

bool CGMEngine::Play()
{
	m_bAudioOver = false;
	std::wstring wstrCurrentFile = m_pAudio->GetCurrentAudio();
	if (L"" == wstrCurrentFile)
	{
		wstrCurrentFile = L"The Minions - Y.M.C.A.mp3";
		m_pAudio->SetCurrentAudio(wstrCurrentFile);
		m_pCharacter->SetMusicDuration(m_pAudio->GetAudioDuration());
	}
	m_pAudio->AudioControl(EGMA_CMD_PLAY);
	m_pCharacter->SetMusicEnable(true);
	return true;
}

/** @brief 暂停 */
bool CGMEngine::Pause()
{
	m_pAudio->AudioControl(EGMA_CMD_PAUSE);
	m_pCharacter->SetMusicEnable(false);
	return true;
}

/** @brief 停止 */
bool CGMEngine::Stop()
{
	m_pAudio->AudioControl(EGMA_CMD_STOP);
	m_pCharacter->SetMusicEnable(false);

	return true;
}

/** @brief 下一首 */
bool CGMEngine::Next()
{
	m_bAudioOver = false;
	_Next(m_ePlayMode);

	return true;
}

bool CGMEngine::SetVolume(const float fVolume)
{
	return m_pAudio->SetVolume(osg::clampBetween(fVolume, 0.0f, 1.0f));
}

float CGMEngine::GetVolume() const
{
	return m_pAudio->GetVolume();
}

bool CGMEngine::SetPlayMode(EGMA_MODE eMode)
{
	m_ePlayMode = eMode;
	return true;
}

std::wstring CGMEngine::GetAudioName() const
{
	return m_pAudio->GetCurrentAudio();
}

bool CGMEngine::SetAudioCurrentTime(const int iTime)
{
	m_pCharacter->SetMusicCurrentTime(iTime);
	return m_pAudio->SetAudioCurrentTime(iTime);
}

int CGMEngine::GetAudioCurrentTime() const
{
	return m_pAudio->GetAudioCurrentTime();
}

int CGMEngine::GetAudioDuration() const
{
	return m_pAudio->GetAudioDuration();
}

bool CGMEngine::IsAudioOver() const
{
	return m_pAudio->IsAudioOver();
}

void CGMEngine::Welcome()
{
	m_pAudio->Welcome();
}

bool CGMEngine::IsWelcomeFinished() const
{
	return m_pAudio->IsWelcomeFinished();
}

CGMViewWidget* CGMEngine::CreateViewWidget(QWidget* parent)
{
	GM_Viewer = new CGMViewWidget(GM_View, parent);
	m_pPost->CreatePost(m_pSceneTex.get(), m_pBackgroundTex.get(), m_pForegroundTex.get());
	if (EGMRENDER_LOW != m_pConfigData->eRenderQuality)
	{
		//m_pPost->SetVolumeEnable(true, m_pGalaxy->GetTAATex());
	}
	return GM_Viewer.get();
}

/** @brief 加载配置 */
bool CGMEngine::_LoadConfig()
{
	CGMXml hXML;
	hXML.Load(g_strGMConfigFile, "Config");
	m_pConfigData = new SGMConfigData;

	// 解析系统配置
	CGMXmlNode sNode = hXML.GetChild("System");

	m_pConfigData->strCorePath = sNode.GetPropStr("corePath", m_pConfigData->strCorePath.c_str());
	m_pConfigData->strMediaPath = sNode.GetPropWStr("mediaPath", m_pConfigData->strMediaPath.c_str());
	m_pConfigData->eRenderQuality = EGMRENDER_QUALITY(sNode.GetPropInt("renderQuality", m_pConfigData->eRenderQuality));
	m_pConfigData->fFovy = sNode.GetPropFloat("fovy", m_pConfigData->fFovy);
	m_pConfigData->bWallpaper = sNode.GetPropBool("wallpaper", m_pConfigData->bWallpaper);

	return true;
}

void CGMEngine::_InitBackground()
{
	m_pBackgroundTex = new osg::Texture2D();
	m_pBackgroundTex->setTextureSize(m_pConfigData->iScreenWidth, m_pConfigData->iScreenHeight);
	m_pBackgroundTex->setInternalFormat(GL_RGBA8);
	m_pBackgroundTex->setSourceFormat(GL_RGBA);
	m_pBackgroundTex->setSourceType(GL_UNSIGNED_BYTE);
	m_pBackgroundTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	m_pBackgroundTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	m_pBackgroundTex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	m_pBackgroundTex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	m_pBackgroundTex->setBorderColor(osg::Vec4d(0, 0, 0, 0));
	m_pBackgroundTex->setDataVariance(osg::Object::DYNAMIC);
	m_pBackgroundTex->setResizeNonPowerOfTwoHint(false);

	m_pKernelData->pBackgroundCam = new osg::Camera;
	m_pKernelData->pBackgroundCam->setName("backgroundCamera");
	m_pKernelData->pBackgroundCam->setReferenceFrame(osg::Transform::ABSOLUTE_RF_INHERIT_VIEWPOINT);
	m_pKernelData->pBackgroundCam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_pKernelData->pBackgroundCam->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
	m_pKernelData->pBackgroundCam->setViewport(0, 0, m_pConfigData->iScreenWidth, m_pConfigData->iScreenHeight);
	m_pKernelData->pBackgroundCam->setRenderOrder(osg::Camera::PRE_RENDER, 0);
	m_pKernelData->pBackgroundCam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	m_pKernelData->pBackgroundCam->attach(osg::Camera::COLOR_BUFFER, m_pBackgroundTex.get());
	m_pKernelData->pBackgroundCam->setAllowEventFocus(false);
	m_pKernelData->pBackgroundCam->setProjectionMatrixAsPerspective(
		m_pConfigData->fFovy,
		double(m_pConfigData->iScreenWidth) / double(m_pConfigData->iScreenHeight),
		2.0, 2e4);
	m_pKernelData->pBackgroundCam->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	GM_Root->addChild(m_pKernelData->pBackgroundCam.get());
}

void CGMEngine::_InitForeground()
{
	m_pForegroundTex = new osg::Texture2D();
	m_pForegroundTex->setTextureSize(m_pConfigData->iScreenWidth, m_pConfigData->iScreenHeight);
	m_pForegroundTex->setInternalFormat(GL_RGBA8);
	m_pForegroundTex->setSourceFormat(GL_RGBA);
	m_pForegroundTex->setSourceType(GL_UNSIGNED_BYTE);
	m_pForegroundTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	m_pForegroundTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	m_pForegroundTex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	m_pForegroundTex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	m_pForegroundTex->setBorderColor(osg::Vec4d(0, 0, 0, 0));
	m_pForegroundTex->setDataVariance(osg::Object::DYNAMIC);
	m_pForegroundTex->setResizeNonPowerOfTwoHint(false);
	
	m_pKernelData->pForegroundCam = new osg::Camera;
	m_pKernelData->pForegroundCam->setName("foregroundCamera");
	m_pKernelData->pForegroundCam->setReferenceFrame(osg::Transform::ABSOLUTE_RF_INHERIT_VIEWPOINT);
	m_pKernelData->pForegroundCam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_pKernelData->pForegroundCam->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
	m_pKernelData->pForegroundCam->setViewport(0, 0, m_pConfigData->iScreenWidth, m_pConfigData->iScreenHeight);
	m_pKernelData->pForegroundCam->setRenderOrder(osg::Camera::PRE_RENDER, 0);
	m_pKernelData->pForegroundCam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	m_pKernelData->pForegroundCam->attach(osg::Camera::COLOR_BUFFER, m_pForegroundTex.get());
	m_pKernelData->pForegroundCam->setAllowEventFocus(false);
	m_pKernelData->pForegroundCam->setProjectionMatrixAsPerspective(
		m_pConfigData->fFovy,
		double(m_pConfigData->iScreenWidth) / double(m_pConfigData->iScreenHeight),
		2.0, 2e4);
	m_pKernelData->pForegroundCam->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	GM_Root->addChild(m_pKernelData->pForegroundCam.get());
}

void CGMEngine::_Next(const EGMA_MODE eMode)
{
	std::wstring wstrCurrentFile = L"";

	switch (eMode)
	{
	case EGMA_MOD_SINGLE:
	{
		m_pAudio->AudioControl(EGMA_CMD_PLAY);
	}
	break;
	//case EGMA_MOD_CIRCLE:
	//{
	//	m_pAudio->AudioControl(EGMA_CMD_CLOSE);

	//	int iMaxNum = m_pDataManager->GetAudioNum();
	//	unsigned int iNext = (m_pDataManager->GetUID() + 1) % iMaxNum;// wrong to do
	//	wstrCurrentFile = m_pDataManager->FindAudio(iNext);

	//	m_pAudio->SetCurrentAudio(wstrCurrentFile);
	//	m_pAudio->AudioControl(EGMA_CMD_OPEN);
	//	m_pAudio->AudioControl(EGMA_CMD_PLAY);

	//	SGMGalaxyCoord vGC = m_pDataManager->GetGalaxyCoord(wstrCurrentFile);
	//	double fWorldX, fWorldY;
	//	_GalaxyCoord2World(vGC.x, vGC.y, fWorldX, fWorldY);
	//	m_pGalaxy->SetCurrentStar(osg::Vec3f(fWorldX, fWorldY, 0.0f), wstrCurrentFile);
	//}
	//break;
	//case EGMA_MOD_RANDOM:
	//{
	//	m_pAudio->AudioControl(EGMA_CMD_CLOSE);

	//	int iMaxNum = m_pDataManager->GetAudioNum();
	//	std::uniform_int_distribution<> iPseudoNoise(0, iMaxNum-1);
	//	int iNext = iPseudoNoise(m_iRandom);
	//	wstrCurrentFile = m_pDataManager->FindAudio(iNext);
	//	while (L"" == wstrCurrentFile)
	//	{
	//		iNext = iPseudoNoise(m_iRandom);
	//		wstrCurrentFile = m_pDataManager->FindAudio(iNext);
	//	}
	//	m_pAudio->SetCurrentAudio(wstrCurrentFile);
	//	m_pAudio->AudioControl(EGMA_CMD_OPEN);
	//	m_pAudio->AudioControl(EGMA_CMD_PLAY);

	//	SGMGalaxyCoord vGC = m_pDataManager->GetGalaxyCoord(wstrCurrentFile);
	//	double fWorldX, fWorldY;
	//	_GalaxyCoord2World(vGC.x, vGC.y, fWorldX, fWorldY);
	//	m_pGalaxy->SetCurrentStar(osg::Vec3f(fWorldX, fWorldY, 0.0f), wstrCurrentFile);
	//}
	//break;
	case EGMA_MOD_ORDER:
	{
		m_pAudio->AudioControl(EGMA_CMD_CLOSE);
	}
	break;
	default:
	{
		m_pAudio->AudioControl(EGMA_CMD_CLOSE);
	}
	break;
	}
}

void CGMEngine::_InnerUpdate(const float updateStep)
{
	// 由于可能在同一帧内出现先设置开启音乐后又设置关闭音乐的情况
	// 所以在这里需要两次判断音乐是否结束
	if (m_pAudio->IsAudioOver())
	{
		if (m_bAudioOver && m_pCharacter->GetMusicEnable())
		{
			m_pCharacter->SetMusicEnable(false);
		}
		m_bAudioOver = true;
	}
}

bool CGMEngine::_UpdateLater(const double dDeltaTime)
{
	// background camera
	osg::Vec3d vEye, vCenter, vUp;
	GM_View->getCamera()->getViewMatrixAsLookAt(vEye, vCenter, vUp);
	osg::Vec3d vBackCamDir = vCenter - vEye;
	m_pKernelData->pBackgroundCam->setViewMatrixAsLookAt(osg::Vec3d(0, 0, 0), vBackCamDir, vUp);
	//前景相机和主相机有相同的姿态
	m_pKernelData->pForegroundCam->setViewMatrixAsLookAt(vEye, vCenter, vUp);
	double fFovy, fAspectRatio, fZNear, fZFar;
	GM_View->getCamera()->getProjectionMatrixAsPerspective(fFovy, fAspectRatio, fZNear, fZFar);
	m_pKernelData->pBackgroundCam->setProjectionMatrixAsPerspective(fFovy, fAspectRatio, fZNear, fZFar);
	m_pKernelData->pForegroundCam->setProjectionMatrixAsPerspective(fFovy, fAspectRatio, fZNear, fZFar);

	GM_LIGHT.UpdatePost(dDeltaTime);
	GM_UNIFORM.UpdatePost(dDeltaTime);

	m_pPost->UpdatePost(dDeltaTime);
	m_pTerrain->UpdatePost(dDeltaTime);
	m_pModel->UpdatePost(dDeltaTime);
	m_pCharacter->UpdatePost(dDeltaTime);

	return true;
}