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
#pragma once

#include "steam/steam_api.h"

enum EAchievements
{
	ACH_DURATION_ONE_HOUR = 0,
	ACH_DURATION_TEN_HOURS = 1,
	ACH_DANCE = 2
};

struct Achievement_t
{
	EAchievements m_eAchievementID;
	const char* m_pchAchievementID;
	char m_rgchName[128];
	char m_rgchDescription[256];
	bool m_bAchieved;
	int m_iIconImage;
};

class ISteamUser;

class CGMStatsAndAchievements
{
public:
	// Constructor
	CGMStatsAndAchievements();

	// Run a frame. Does not need to run at full frame rate.
	void RunFrame();

	// accessors
	double GetGameDurationSeconds() { return m_flGameDurationSeconds; }

	STEAM_CALLBACK(CGMStatsAndAchievements, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	STEAM_CALLBACK(CGMStatsAndAchievements, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);
	// 用于接收 RequestCurrentStats 的回调
	STEAM_CALLBACK(CGMStatsAndAchievements, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);

private:

	void LoadUserStats();

	// Determine if we get this achievement now
	void EvaluateAchievement(Achievement_t& achievement);
	void UnlockAchievement(Achievement_t& achievement);

	// Store stats
	void StoreStatsIfNecessary();

	// our GameID
	CGameID m_GameID;

	// Steam User interface
	ISteamUser* m_pSteamUser;

	// Steam UserStats interface
	ISteamUserStats* m_pSteamUserStats;

	// Did we get the stats from Steam?
	bool m_bStatsValid;
	// 防止重复请求当前 stats
	bool m_bRequestedStats;
	// Should we store stats this frame?
	bool m_bStoreStats;

	// Current Stat details
	double m_flGameDurationSeconds;
	// accumulate seconds since last StoreStats trigger
	double m_flSecondsSinceLastStore;
};