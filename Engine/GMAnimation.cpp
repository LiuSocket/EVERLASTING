//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2040, LT
/// All rights reserved.
///
/// @file		GMAnimation.cpp
/// @brief		GMEngine - Animation manager
/// @version	1.0
/// @author		LiuTao
/// @date		2024.1.11
//////////////////////////////////////////////////////////////////////////

#include "GMAnimation.h"
#include "GMNodeVisitor.h"
#include "GMCommon.h"

using namespace GM;

/*************************************************************************
 Macro Defines
*************************************************************************/
#define  ANIM_LIST				_manager->getAnimationList()		// ��ȡ����

namespace GM
{
	/*
	*  @brief �������������̳���osgAnimation::BasicAnimationManager
	*/
	class CGMBasicAnimationManager : public osgAnimation::BasicAnimationManager
	{
	public:

		CGMBasicAnimationManager() {}

		CGMBasicAnimationManager(const osgAnimation::AnimationManagerBase& b, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
			: osgAnimation::BasicAnimationManager(b, copyop) {}

		virtual ~CGMBasicAnimationManager() {}

		// add pauseTime to resume animation
		void resumeAnimation(osgAnimation::Animation* pAnimation, int priority = 0, float weight = 1.0)
		{
			if (!findAnimation(pAnimation))
				return;

			if (fPauseTimeMap.end() == fPauseTimeMap.find(pAnimation->getName())) return;

			if (isPlaying(pAnimation))
				stopAnimation(pAnimation);

			_animationsPlaying[priority].push_back(pAnimation);
			// for debug
			//std::cout << "player Animation " << pAnimation->getName() << " at " << _lastUpdate << std::endl;
			pAnimation->setStartTime(_lastUpdate - fPauseTimeMap.at(pAnimation->getName()));
			pAnimation->setWeight(weight);
		}

		// add pauseTime to resume animation
		bool pauseAnimation(osgAnimation::Animation* pAnimation)
		{
			// search though the layer and remove animation
			for (auto& iterAnim : _animationsPlaying)
			{
				osgAnimation::AnimationList& list = iterAnim.second;
				for (auto it = list.begin(); it != list.end(); ++it)
					if ((*it) == pAnimation)
					{
						(*it)->resetTargets();
						list.erase(it);
						double fPauseTime = fmod(_lastUpdate - pAnimation->getStartTime(), pAnimation->getDuration());
						fPauseTimeMap.insert(std::pair< std::string, double>(pAnimation->getName(), fPauseTime));
						return true;
					}
			}
			return false;
		}

	protected:
		std::map<std::string, double>	fPauseTimeMap; // ������ͣ������ʱ���ӳ���
	};

	/*
	* @brief ����������������
	*/
	struct AnimationManagerFinder : public osg::NodeVisitor
	{
		osg::ref_ptr<CGMBasicAnimationManager> _am;
		AnimationManagerFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
		void apply(osg::Node& node) {
			if (_am.valid())
				return;
			if (node.getUpdateCallback()) {
				osgAnimation::AnimationManagerBase* b = dynamic_cast<osgAnimation::AnimationManagerBase*>(node.getUpdateCallback());
				if (b) {
					_am = new CGMBasicAnimationManager(*b);
					return;
				}
			}
			traverse(node);
		}
	};

	/*
	*  @brief ������������ͬһ��ģ���ļ����Թ���һ������������Ŀǰ��û��ʵ�ֹ��ã�������������ͬʱ���Ŷ������
	*/
	class CAnimationPlayer : public osg::Referenced
	{
	public:
		CAnimationPlayer() : _manager(nullptr), _focus(0) {}

		// ���ģ�����ƣ�Ŀǰֻ�����һ����
		bool addModel(const std::string& strModelName)
		{
			if (_modelNameVec.end() != find(_modelNameVec.begin(), _modelNameVec.end(), strModelName)) return false;

			_modelNameVec.push_back(strModelName);
			return true;
		}

		// �ж�_modelNameVec���Ƿ��Ѿ�����˸�ģ������
		bool hasModel(const std::string& strModelName)
		{
			return _modelNameVec.end() != find(_modelNameVec.begin(), _modelNameVec.end(), strModelName);
		}

		// ��Ӷ���������
		bool addManager(CGMBasicAnimationManager* manager)
		{
			_manager = manager;
			for (auto& it : ANIM_LIST)
			{
				_animPriorityVec.push_back(0);
			}
			return true;
		}

		/* @brief ���ŵ�ǰ�۽��Ķ��������ֻ��һ�������ڲ��ţ����޸�Ȩ����Ч */
		bool play()
		{
			if (_focus < ANIM_LIST.size())
			{
				_manager->playAnimation(ANIM_LIST.at(_focus).get());
				return true;
			}
			return false;
		}
		/* @brief ����ָ���Ķ��������ֻ��һ�������ڲ��ţ����޸�Ȩ����Ч */
		bool play(const std::string& name, float weight)
		{
			for (unsigned int i = 0; i < ANIM_LIST.size(); i++)
			{
				if (ANIM_LIST.at(i)->getName() == name)
					_focus = i;
			}
			_manager->playAnimation(ANIM_LIST.at(_focus).get(), _animPriorityVec[_focus], weight);
			return true;
		}

		/* @brief ֹͣ���ж�����ͣ�ڵ�ǰλ�� */
		void stop()
		{
			_manager->stopAll();
		}
		/* @brief ָֹͣ���Ķ��� */
		bool stop(const std::string& name)
		{
			for (unsigned int i = 0; i < ANIM_LIST.size(); i++)
			{
				if (ANIM_LIST.at(i)->getName() == name)
					_focus = i;
			}
			_manager->stopAnimation(ANIM_LIST.at(_focus).get());
			return true;
		}

		/* @brief ��ͣ���ж��� */
		bool pause()
		{
			for (unsigned int i = 0; i < ANIM_LIST.size(); i++)
			{
				// _animPauseTimeVec�����ڼ�������ʱ�Ŀ�ʼʱ��
				_manager->pauseAnimation(ANIM_LIST.at(_focus).get());
				return true;
			}
			return false;
		}
		/* @brief ��ָͣ���Ķ��� */
		bool pause(const std::string& name)
		{
			for (unsigned int i = 0; i < ANIM_LIST.size(); i++)
			{
				if (ANIM_LIST.at(i)->getName() == name)
					_focus = i;
			}
			// ���ڼ�������ʱ�Ŀ�ʼʱ��
			_manager->pauseAnimation(ANIM_LIST.at(_focus).get());

			return true;
		}

		/* @brief �������ŵ�ǰ�۽��Ķ��� */
		bool resume()
		{
			if (_focus < ANIM_LIST.size())
			{
				_manager->resumeAnimation(ANIM_LIST.at(_focus).get(), _animPriorityVec[_focus], 1.0);
				return true;
			}
			return false;
		}
		/* @brief ��������ָ���Ķ��� */
		bool resume(const std::string& name)
		{
			for (unsigned int i = 0; i < ANIM_LIST.size(); i++)
			{
				if (ANIM_LIST.at(i)->getName() == name)
					_focus = i;
			}
			_manager->resumeAnimation(ANIM_LIST.at(_focus).get(), _animPriorityVec[_focus], 1.0);

			return true;
		}

		/* @brief ���õ�ǰ�۽��Ķ����Ĳ������ȼ� */
		bool setPriority(const int iPriority)
		{
			if (_focus < ANIM_LIST.size())
			{
				_animPriorityVec[_focus] = iPriority;
				return true;
			}
			return false;
		}
		/* @brief ����ָ�������Ĳ������ȼ� */
		bool setPriority(const std::string& name, const int iPriority)
		{
			for (unsigned int i = 0; i < ANIM_LIST.size(); i++)
			{
				if (ANIM_LIST.at(i)->getName() == name)
				{
					_animPriorityVec[i] = iPriority;
					return true;
				}
			}
			return false;
		}
		bool getPriority(const std::string& name, int& iPriority)
		{
			for (unsigned int i = 0; i < ANIM_LIST.size(); i++)
			{
				if (ANIM_LIST.at(i)->getName() == name)
				{
					iPriority = _animPriorityVec[i];
					return true;
				}
			}
			return false;
		}

		/* @brief ���õ�ǰ�۽��Ķ����Ĳ���ʱ��, ��λ���� */
		bool setDuration(const double fDuration)
		{
			if (_focus < ANIM_LIST.size())
			{
				ANIM_LIST.at(_focus)->setDuration(fDuration);
				return true;
			}
			return false;
		}

		/* @brief ����ָ�������Ĳ���ʱ��, ��λ���� */
		bool setDuration(const std::string& name, const double fDuration)
		{
			for (unsigned int i = 0; i < ANIM_LIST.size(); i++)
			{
				if (ANIM_LIST.at(i)->getName() == name)
				{
					ANIM_LIST.at(i)->setDuration(fDuration);
					return true;
				}
			}
			return false;
		}
		bool getDuration(const std::string& name, double& fDuration)
		{
			for (unsigned int i = 0; i < ANIM_LIST.size(); i++)
			{
				if (ANIM_LIST.at(i)->getName() == name)
				{
					fDuration = ANIM_LIST.at(i)->getDuration();
					if (fDuration == 0)
					{
						ANIM_LIST.at(i)->computeDuration();
						fDuration = ANIM_LIST.at(i)->getDuration();
					}
					return true;
				}
			}
			return false;
		}

		/* @brief ���õ�ǰ�۽��Ķ����Ĳ���ģʽ */
		bool setPlayMode(osgAnimation::Animation::PlayMode ePlayMode)
		{
			if (_focus < ANIM_LIST.size())
			{
				ANIM_LIST.at(_focus)->setPlayMode(ePlayMode);
				return true;
			}
			return false;
		}
		/* @brief ����ָ�������Ĳ���ģʽ */
		bool setPlayMode(const std::string& name, osgAnimation::Animation::PlayMode ePlayMode)
		{
			for (unsigned int i = 0; i < ANIM_LIST.size(); i++)
			{
				if (ANIM_LIST.at(i)->getName() == name)
				{
					ANIM_LIST.at(i)->setPlayMode(ePlayMode);
					return true;
				}
			}
			return false;
		}
		/* @brief ��ȡָ�������Ĳ���ģʽ */
		bool getPlayMode(const std::string& name, osgAnimation::Animation::PlayMode& ePlayMode)
		{
			for (unsigned int i = 0; i < ANIM_LIST.size(); i++)
			{
				if (ANIM_LIST.at(i)->getName() == name)
				{
					ePlayMode = ANIM_LIST.at(i)->getPlayMode();
					return true;
				}
			}
			return true;
		}

		const std::string& getCurrentAnimationName() const
		{
			return ANIM_LIST.at(_focus)->getName();
		}

		void getAnimationList(std::vector<std::string>& vAnimationList) const
		{
			vAnimationList.clear();
			for (unsigned int i = 0; i < ANIM_LIST.size(); i++)
			{
				vAnimationList.push_back(ANIM_LIST.at(i)->getName());
			}
		}

	private:
		osg::ref_ptr<CGMBasicAnimationManager>	_manager; // ����������
		std::vector<int>						_animPriorityVec; // �������ȼ�
		std::vector<std::string>				_modelNameVec; // ģ������
		unsigned int							_focus; // ��ǰ��������
	};
}

/*************************************************************************
CGMAnimation Methods
*************************************************************************/

CGMAnimation::CGMAnimation()
{
}

CGMAnimation::~CGMAnimation()
{
}

bool CGMAnimation::Reset()
{
	_ClearPlayer();
	return true;
}

bool CGMAnimation::AddAnimation(const std::string& strName, osg::Node* pNode)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strName);
	if (pAniPlayer) return false;

	AnimationManagerFinder finder;
	pNode->accept(finder);
	if (finder._am.valid())
	{
		pNode->setUpdateCallback(finder._am.get());
		CAnimationPlayer* pPlayer = new CAnimationPlayer;
		pPlayer->addManager(finder._am.get());
		pPlayer->addModel(strName);

		const std::string strPlayerName = strName + "_Player";
		_AddPlayer(strPlayerName, pPlayer);

		return true;
	}
	return false;
}

bool CGMAnimation::RemoveAnimation(const std::string& strName)
{
	const std::string strPlayerName = strName + "_Player";
	_RemovePlayer(strPlayerName);

	return true;
}

bool CGMAnimation::GetAnimationEnable(const std::string& strName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strName);
	return pAniPlayer ? true : false;
}

bool CGMAnimation::SetAnimationDuration(const std::string& strModelName, const float fDuration, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (pAniPlayer)
	{
		if ("" == strAnimationName)
		{
			pAniPlayer->setDuration(fDuration);
		}
		else
		{
			pAniPlayer->setDuration(strAnimationName, fDuration);
		}

		return true;
	}
	else
	{
		return false;
	}
}

float CGMAnimation::GetAnimationDuration(const std::string& strModelName, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (!pAniPlayer) return 0.0;
	double duration = 0.0;
	pAniPlayer->getDuration(strAnimationName, duration);
	return duration;
}

bool CGMAnimation::SetAnimationMode(const std::string& strModelName, EGMPlayMode ePlayMode, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (pAniPlayer)
	{
		if ("" == strAnimationName)
		{
			pAniPlayer->setPlayMode((osgAnimation::Animation::PlayMode)ePlayMode);
		}
		else
		{
			pAniPlayer->setPlayMode(strAnimationName, (osgAnimation::Animation::PlayMode)ePlayMode);
		}

		return true;
	}
	else
	{
		return false;
	}
}

EGMPlayMode CGMAnimation::GetAnimationMode(const std::string& strModelName, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (!pAniPlayer) return EGMPlayMode::EGM_PLAY_ONCE;
	osgAnimation::Animation::PlayMode playermode;
	pAniPlayer->getPlayMode(strAnimationName, playermode);
	return (EGMPlayMode)playermode;
}

bool CGMAnimation::SetAnimationPriority(const std::string& strModelName, const int iPriority, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (pAniPlayer)
	{
		if ("" == strAnimationName)
		{
			pAniPlayer->setPriority(iPriority);
		}
		else
		{
			pAniPlayer->setPriority(strAnimationName, iPriority);
		}

		return true;
	}
	else
	{
		return false;
	}
}

int CGMAnimation::GetAnimationPriority(const std::string& strModelName, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (!pAniPlayer) return 0;
	int priority = 0;
	pAniPlayer->getPriority(strAnimationName, priority);
	return priority;
}

bool CGMAnimation::SetAnimationPlay(const std::string& strModelName, const float fWeight, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (pAniPlayer)
	{
		if ("" == strAnimationName)
		{
			if (fWeight > 0)
				pAniPlayer->play();
			else
				pAniPlayer->stop();
		}
		else
		{
			if (fWeight > 0)
				pAniPlayer->play(strAnimationName, fWeight);
			else
				pAniPlayer->stop(strAnimationName);
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool CGMAnimation::SetAnimationPause(const std::string& strModelName, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (pAniPlayer)
	{
		if ("" == strAnimationName) // ��ͣ���ж���
		{
			pAniPlayer->pause();
		}
		else // ��ͣĳһ������
		{
			pAniPlayer->pause(strAnimationName);
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool CGMAnimation::SetAnimationResume(const std::string& strModelName, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (pAniPlayer)
	{
		if ("" != strAnimationName) // �������Ʋ���Ϊ��
		{
			pAniPlayer->resume(strAnimationName);
			return true;
		}
	}

	return false;
}

bool CGMAnimation::GetAnimationList(const std::string& strModelName, std::vector<std::string>& vAnimationList)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (!pAniPlayer) return false;

	pAniPlayer->getAnimationList(vAnimationList);
	return true;
}

CAnimationPlayer* CGMAnimation::_GetPlayerByModelName(const std::string& strModelName)
{
	std::string strPlayerName = _GetPlayerName(strModelName);
	if (strPlayerName == "") return nullptr;

	return _GetPlayer(strPlayerName);
}

bool CGMAnimation::_AddPlayer(const std::string& strPlayerName, CAnimationPlayer* pPlayer)
{
	// ���������Ʋ���Ϊ��
	if (strPlayerName == "") return false;

	if (m_playerMap.end() == m_playerMap.find(strPlayerName))
	{
		m_playerMap[strPlayerName] = pPlayer;
		return true;
	}
	return false;
}

bool CGMAnimation::_RemovePlayer(const std::string& strPlayerName)
{
	// ���������Ʋ���Ϊ��
	if (strPlayerName == "") return false;

	auto iter = m_playerMap.find(strPlayerName);
	if (iter != m_playerMap.end())
	{
		m_playerMap.erase(iter);
		return true;
	}
	return false;
}

bool CGMAnimation::_ClearPlayer()
{
	auto iter = m_playerMap.begin();
	while (iter != m_playerMap.end())
	{
		m_playerMap.erase(iter++);
	}
	return true;
}

/* ���ݲ��������ƻ�ȡ���������� */
CAnimationPlayer* CGMAnimation::_GetPlayer(const std::string& strPlayerName)
{
	auto iter = m_playerMap.find(strPlayerName);

	if (iter == m_playerMap.end())
		return nullptr;
	else
		return m_playerMap[strPlayerName];
}

/* ����ģ�����ƻ�ȡ�������������� */
std::string CGMAnimation::_GetPlayerName(const std::string& strModelName)
{
	for (auto it = m_playerMap.begin(); it != m_playerMap.end(); it++)
	{
		if (it->second->hasModel(strModelName))
			return it->first;
	}
	return "";
}