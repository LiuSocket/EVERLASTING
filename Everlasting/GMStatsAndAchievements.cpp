//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2026~2046, LiuTao
/// All rights reserved.
///
/// @file		GMStatsAndAchievements.h
/// @brief		统计和成就
/// @version	1.0
/// @author		LiuTao
/// @date		2026.01.31
//////////////////////////////////////////////////////////////////////////

#include "GMStatsAndAchievements.h"
#include "../Engine/GMEngine.h"
#include <iostream>

#define _ACH_ID( id, name ) { id, #id, name, "", 0, 0 }

Achievement_t g_rgAchievements[] = 
{
	_ACH_ID(ACH_DURATION_ONE_HOUR, "OneHour" ),
	_ACH_ID(ACH_DURATION_TEN_HOURS, "TenHours" ),
	_ACH_ID(ACH_DANCE, "Dance" )
};

using namespace GM;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
#pragma warning( push )
//  warning C4355: 'this' : used in base member initializer list
//  This is OK because it's warning on setting up the Steam callbacks, they won't use this until after construction is done
#pragma warning( disable : 4355 ) 
CGMStatsAndAchievements::CGMStatsAndAchievements() :
	m_pSteamUser(nullptr),
	m_pSteamUserStats(nullptr),
	m_GameID(SteamUtils()->GetAppID()),
	m_CallbackUserStatsStored(this, &CGMStatsAndAchievements::OnUserStatsStored),
	m_CallbackAchievementStored(this, &CGMStatsAndAchievements::OnAchievementStored),
	m_CallbackUserStatsReceived(this, &CGMStatsAndAchievements::OnUserStatsReceived),
	m_bStatsValid(false),
	m_bRequestedStats(false),
	m_bStoreStats(false),
	m_flGameDurationSeconds(0.0),
	m_flSecondsSinceLastStore(0.0)
{
	m_pSteamUser = SteamUser();
	m_pSteamUserStats = SteamUserStats();
}
#pragma warning( pop )

//-----------------------------------------------------------------------------
// Purpose: Run a frame for the CGMStatsAndAchievements. does not need to run at
// full frame rate.
//-----------------------------------------------------------------------------
void CGMStatsAndAchievements::RunFrame()
{
	if ( !m_bStatsValid )
		LoadUserStats();

	double flGameDurationSeconds = GM_ENGINE.GetElapsedTimeSeconds();
	double flGameDurationDeltaSeconds = flGameDurationSeconds - m_flGameDurationSeconds;
	m_flSecondsSinceLastStore += flGameDurationDeltaSeconds;
	m_flGameDurationSeconds = flGameDurationSeconds;

	// Evaluate achievements
	for ( int iAch = 0; iAch < ARRAYSIZE( g_rgAchievements ); ++iAch )
	{
		EvaluateAchievement( g_rgAchievements[iAch] );
	}

	// Periodically update Steam stat for total playtime (every 30 seconds)
	static double STORE_INTERVAL_SECONDS = 30.0;
	if (m_flSecondsSinceLastStore >= STORE_INTERVAL_SECONDS)
	{
		if (m_pSteamUserStats)
		{
			int totalSeconds = static_cast<int>(m_flGameDurationSeconds);
			m_pSteamUserStats->SetStat("TotalPlaytimeSeconds", totalSeconds);
			m_bStoreStats = true;
			m_flSecondsSinceLastStore = 0.0;
		}
	}

	// Store stats
	StoreStatsIfNecessary();
}

//-----------------------------------------------------------------------------
// Purpose: see if we should unlock this achievement
//-----------------------------------------------------------------------------
void CGMStatsAndAchievements::EvaluateAchievement( Achievement_t &achievement )
{
	// Already have it?
	if ( achievement.m_bAchieved )
		return;

	switch ( achievement.m_eAchievementID )
	{
	case ACH_DURATION_ONE_HOUR:
	{
		// 1 hour = 3600 seconds
		if (m_flGameDurationSeconds >= 3600.0)
			UnlockAchievement(achievement);
	}
	break;
	case ACH_DURATION_TEN_HOURS:
	{
		// 10 hours = 36000 seconds
		if (m_flGameDurationSeconds >= 36000.0)
			UnlockAchievement(achievement);
	}
	break;
	case ACH_DANCE:
	{
		UnlockAchievement(achievement);
	}
	break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Unlock this achievement
//-----------------------------------------------------------------------------
void CGMStatsAndAchievements::UnlockAchievement( Achievement_t &achievement )
{
	achievement.m_bAchieved = true;

	// the icon may change once it's unlocked
	achievement.m_iIconImage = 0;

	// mark it down
	m_pSteamUserStats->SetAchievement( achievement.m_pchAchievementID );

	// Store stats end of frame
	m_bStoreStats = true;
}

//-----------------------------------------------------------------------------
// Purpose: Store stats in the Steam database
//-----------------------------------------------------------------------------
void CGMStatsAndAchievements::StoreStatsIfNecessary()
{
	if ( m_bStoreStats )
	{
		// already set any achievements in UnlockAchievement

		//// set stats
		//m_pSteamUserStats->SetStat( "NumGames", m_nTotalGamesPlayed );
		//m_pSteamUserStats->SetStat( "NumWins", m_nTotalNumWins );
		//m_pSteamUserStats->SetStat( "NumLosses", m_nTotalNumLosses );
		//m_pSteamUserStats->SetStat( "FeetTraveled", m_flTotalFeetTraveled );
		//m_pSteamUserStats->SetStat( "MaxFeetTraveled", m_flMaxFeetTraveled );
		//// Update average feet / second stat
		//m_pSteamUserStats->UpdateAvgRateStat( "AverageSpeed", m_flGameFeetTraveled, m_flGameDurationSeconds );
		//// The averaged result is calculated for us
		//m_pSteamUserStats->GetStat( "AverageSpeed", &m_flAverageSpeed );

		bool bSuccess = m_pSteamUserStats->StoreStats();
		// If this failed, we never sent anything to the server, try
		// again later.
		m_bStoreStats = !bSuccess;
	}
}

//-----------------------------------------------------------------------------
// Purpose: We have stats data from Steam. It is authoritative, so update
//			our data with those results now.
//-----------------------------------------------------------------------------
void CGMStatsAndAchievements::LoadUserStats()
{
	if ( !m_pSteamUserStats )
		return;

	// 如果还没请求过当前 stats，就请求一次并等待回调处理
	if (!m_bRequestedStats)
	{
		if (m_pSteamUserStats->RequestUserStats(m_pSteamUser->GetSteamID()))
		{
			std::cout << "RequestCurrentStats sent, waiting for UserStatsReceived_t callback." << std::endl;
			m_bRequestedStats = true;
		}
		else
		{
			std::cout << "RequestCurrentStats failed to send." << std::endl;
		}
		return;
	}

	// 若已接收到 stats（m_bStatsValid == true），下面的读取逻辑可执行。
	if (!m_bStatsValid)
	{
		// 尚未收到数据（回调未到），什么都不做
		return;
	}

	// load achievements
	for ( int iAch = 0; iAch < ARRAYSIZE( g_rgAchievements ); ++iAch )
	{
		Achievement_t &ach = g_rgAchievements[iAch];
		m_pSteamUserStats->GetAchievement( ach.m_pchAchievementID, &ach.m_bAchieved );
		std::cout << ach.m_rgchName << m_pSteamUserStats->GetAchievementDisplayAttribute(ach.m_pchAchievementID, "name");
		std::cout << ach.m_rgchDescription << m_pSteamUserStats->GetAchievementDisplayAttribute(ach.m_pchAchievementID, "desc");
		std::cout << std::endl;
	}

	// load stats
	//m_pSteamUserStats->GetStat( "NumGames", &m_nTotalGamesPlayed );
	// load total playtime stat if present
	int totalSeconds = 0;
	if (m_pSteamUserStats->GetStat("TotalPlaytimeSeconds", &totalSeconds))
	{
		m_flGameDurationSeconds = static_cast<double>(totalSeconds);
	}

	m_bStatsValid = true;
}


//-----------------------------------------------------------------------------
// Purpose: Our stats data was stored!
//-----------------------------------------------------------------------------
void CGMStatsAndAchievements::OnUserStatsStored( UserStatsStored_t *pCallback )
{
	// we may get callbacks for other games' stats arriving, ignore them
	if ( m_GameID.ToUint64() == pCallback->m_nGameID )
	{
		if ( k_EResultOK == pCallback->m_eResult )
		{
			std::cout << "StoreStats - success" << std::endl;
		}
		else if ( k_EResultInvalidParam == pCallback->m_eResult )
		{
			// One or more stats we set broke a constraint. They've been reverted,
			// and we should re-iterate the values now to keep in sync.
			std::cout << "StoreStats - some failed to validate" << std::endl;
			LoadUserStats();
		}
		else
		{
			char buffer[128];
			std::cout << "StoreStats - failed," << buffer << pCallback->m_eResult << std::endl;
			buffer[ sizeof(buffer) - 1 ] = 0;
			std::cout << buffer << std::endl;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: An achievement was stored
//-----------------------------------------------------------------------------
void CGMStatsAndAchievements::OnAchievementStored( UserAchievementStored_t *pCallback )
{
	// we may get callbacks for other games' stats arriving, ignore them
	if ( m_GameID.ToUint64() == pCallback->m_nGameID )
	{
		//if ( 0 == pCallback->m_nMaxProgress )
		//{
		//	char buffer[128];
		//	sprintf_safe( buffer, "Achievement '%s' unlocked!", pCallback->m_rgchAchievementName );
		//	buffer[ sizeof(buffer) - 1 ] = 0;
		//	OutputDebugString( buffer );
		//}
		//else
		//{
		//	char buffer[128];
		//	sprintf_safe( buffer, "Achievement '%s' progress callback, (%d,%d)\n", 
		//		pCallback->m_rgchAchievementName, pCallback->m_nCurProgress, pCallback->m_nMaxProgress );
		//	buffer[ sizeof(buffer) - 1 ] = 0;
		//	OutputDebugString( buffer );
		//}
	}
}

// 在收到 UserStatsReceived_t 后处理
void CGMStatsAndAchievements::OnUserStatsReceived(UserStatsReceived_t* pCallback)
{
	// 确保是当前游戏的回调
	if (m_GameID.ToUint64() != pCallback->m_nGameID)
		return;

	if (pCallback->m_eResult == k_EResultOK)
	{
		std::cout << "UserStatsReceived - OK" << std::endl;
		// 标记已接收并可读取 stats
		m_bStatsValid = true;

		// 现在可以安全读取 stats / achievements 了
		// 选择直接调用 LoadUserStats 的读取部分，或者复制必要的读取逻辑到这里
		// 为简洁，调用 LoadUserStats 再次进入会发现 m_bRequestedStats 已为 true 且 m_bStatsValid 为 true
		LoadUserStats();
	}
	else
	{
		std::cout << "UserStatsReceived - failed, result: " << pCallback->m_eResult << std::endl;
		// 可选择重试 RequestCurrentStats 或其它错误处理
		m_bRequestedStats = false;
	}
}