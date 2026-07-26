#pragma once
#ifndef TF_PLAYER_STATS_H
#define TF_PLAYER_STATS_H

#ifdef GAME_DLL

#include "cbase.h"

struct TFPlayerStat_t
{
	int iTotalKills;
	int iOneVOneWins;
	int iSecondsPlayed;       // persisted total, NOT including current session
	float flSessionStartTime; // gpGlobals->curtime when this session began

	TFPlayerStat_t()
	{
		iTotalKills = 0;
		iOneVOneWins = 0;
		iSecondsPlayed = 0;
		flSessionStartTime = 0.0f;
	}
};

class CPlayerStatsManager
{
public:
	CPlayerStatsManager();

	void AddKill(CBasePlayer* pPlayer);
	void AddOneVOneWin(CBasePlayer* pPlayer);

	// Includes the current session's elapsed time added to the saved total
	void GetDisplayStats(CBasePlayer* pPlayer, int& iOutSeconds, int& iOutWins, int& iOutKills);

	void OnPlayerConnect(CBasePlayer* pPlayer);
	void OnPlayerDisconnect(CBasePlayer* pPlayer);
	void SaveStats(CBasePlayer* pPlayer);
	void SaveAllStats();

private:
	bool GetSteamIDString(CBasePlayer* pPlayer, char* pszOut, int iBufSize);
	TFPlayerStat_t& GetOrLoadStats(CBasePlayer* pPlayer);
	void LoadStatsFromDisk(const char* pszSteamID, TFPlayerStat_t& out);
	void WriteStatsToDisk(const char* pszSteamID, const TFPlayerStat_t& stats);

	CUtlMap< uint64, TFPlayerStat_t > m_StatsCache;
};

extern CPlayerStatsManager g_PlayerStatsManager;

#endif // GAME_DLL
#endif // TF_PLAYER_STATS_H