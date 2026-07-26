#include "cbase.h"
#include "tf_player_stats.h"

#ifdef GAME_DLL

#include "tf_player.h"
#include "filesystem.h"
#include "KeyValues.h"

CPlayerStatsManager g_PlayerStatsManager;

static const char* STATS_DIR = "cfg/playerstats";

CPlayerStatsManager::CPlayerStatsManager()
	: m_StatsCache(DefLessFunc(uint64))
{
}

bool CPlayerStatsManager::GetSteamIDString(CBasePlayer* pPlayer, char* pszOut, int iBufSize)
{
	if (!pPlayer)
		return false;

	CSteamID steamID;
	if (!pPlayer->GetSteamID(&steamID))
		return false;

	Q_snprintf(pszOut, iBufSize, "%llu", steamID.ConvertToUint64());
	return true;
}

void CPlayerStatsManager::LoadStatsFromDisk(const char* pszSteamID, TFPlayerStat_t& out)
{
	char szPath[MAX_PATH];
	Q_snprintf(szPath, sizeof(szPath), "%s/%s.txt", STATS_DIR, pszSteamID);

	KeyValues* pKV = new KeyValues("PlayerStats");
	if (pKV->LoadFromFile(filesystem, szPath, "MOD"))
	{
		out.iTotalKills = pKV->GetInt("kills", 0);
		out.iOneVOneWins = pKV->GetInt("wins", 0);
		out.iSecondsPlayed = pKV->GetInt("seconds", 0);
	}
	pKV->deleteThis();
}

void CPlayerStatsManager::WriteStatsToDisk(const char* pszSteamID, const TFPlayerStat_t& stats)
{
	char szPath[MAX_PATH];
	Q_snprintf(szPath, sizeof(szPath), "%s/%s.txt", STATS_DIR, pszSteamID);

	KeyValues* pKV = new KeyValues("PlayerStats");
	pKV->SetInt("kills", stats.iTotalKills);
	pKV->SetInt("wins", stats.iOneVOneWins);
	pKV->SetInt("seconds", stats.iSecondsPlayed);
	pKV->SaveToFile(filesystem, szPath, "MOD");
	pKV->deleteThis();
}

TFPlayerStat_t& CPlayerStatsManager::GetOrLoadStats(CBasePlayer* pPlayer)
{
	static TFPlayerStat_t s_dummy;

	char szSteamID[64];
	if (!GetSteamIDString(pPlayer, szSteamID, sizeof(szSteamID)))
		return s_dummy;

	CSteamID steamID;
	pPlayer->GetSteamID(&steamID);
	uint64 nID = steamID.ConvertToUint64();

	int idx = m_StatsCache.Find(nID);
	if (idx == m_StatsCache.InvalidIndex())
	{
		TFPlayerStat_t stats;
		LoadStatsFromDisk(szSteamID, stats);
		stats.flSessionStartTime = gpGlobals->curtime;
		idx = m_StatsCache.Insert(nID, stats);
	}

	return m_StatsCache[idx];
}

void CPlayerStatsManager::OnPlayerConnect(CBasePlayer* pPlayer)
{
	GetOrLoadStats(pPlayer).flSessionStartTime = gpGlobals->curtime;
}

void CPlayerStatsManager::AddKill(CBasePlayer* pPlayer)
{
	if (!pPlayer)
		return;
	GetOrLoadStats(pPlayer).iTotalKills++;
}

void CPlayerStatsManager::AddOneVOneWin(CBasePlayer* pPlayer)
{
	if (!pPlayer)
		return;
	GetOrLoadStats(pPlayer).iOneVOneWins++;
}

void CPlayerStatsManager::GetDisplayStats(CBasePlayer* pPlayer, int& iOutSeconds, int& iOutWins, int& iOutKills)
{
	TFPlayerStat_t& stats = GetOrLoadStats(pPlayer);
	int iSessionSeconds = (int)(gpGlobals->curtime - stats.flSessionStartTime);
	if (iSessionSeconds < 0)
		iSessionSeconds = 0;

	iOutSeconds = stats.iSecondsPlayed + iSessionSeconds;
	iOutWins = stats.iOneVOneWins;
	iOutKills = stats.iTotalKills;
}

void CPlayerStatsManager::SaveStats(CBasePlayer* pPlayer)
{
	char szSteamID[64];
	if (!GetSteamIDString(pPlayer, szSteamID, sizeof(szSteamID)))
		return;

	TFPlayerStat_t& stats = GetOrLoadStats(pPlayer);

	int iSessionSeconds = (int)(gpGlobals->curtime - stats.flSessionStartTime);
	if (iSessionSeconds > 0)
	{
		stats.iSecondsPlayed += iSessionSeconds;
		stats.flSessionStartTime = gpGlobals->curtime;
	}

	WriteStatsToDisk(szSteamID, stats);
}

void CPlayerStatsManager::SaveAllStats()
{
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer* pPlayer = UTIL_PlayerByIndex(i);
		if (pPlayer && pPlayer->IsConnected())
			SaveStats(pPlayer);
	}
}

void CPlayerStatsManager::OnPlayerDisconnect(CBasePlayer* pPlayer)
{
	SaveStats(pPlayer);

	CSteamID steamID;
	if (pPlayer && pPlayer->GetSteamID(&steamID))
	{
		int idx = m_StatsCache.Find(steamID.ConvertToUint64());
		if (idx != m_StatsCache.InvalidIndex())
			m_StatsCache.RemoveAt(idx);
	}
}

CON_COMMAND(stats, "Shows your play stats: time played, 1v1 wins, total kills")
{
	CBasePlayer* pPlayer = UTIL_GetCommandClient();
	if (!pPlayer)
		return;

	int iSeconds, iWins, iKills;
	g_PlayerStatsManager.GetDisplayStats(pPlayer, iSeconds, iWins, iKills);

	int iHours = iSeconds / 3600;
	int iMinutes = (iSeconds % 3600) / 60;
	int iSecs = iSeconds % 60;

	char szMsg[256];
	Q_snprintf(szMsg, sizeof(szMsg),
		"--- Your Stats ---\nTime Played: %d:%02d:%02d\n1v1 Wins: %d\nTotal Kills: %d",
		iHours, iMinutes, iSecs, iWins, iKills);

	ClientPrint(pPlayer, HUD_PRINTTALK, szMsg);

	CSingleUserRecipientFilter filter(pPlayer);
	UserMessageBegin(filter, "PlayerStats");
	WRITE_LONG(iSeconds);
	WRITE_LONG(iWins);
	WRITE_LONG(iKills);
	MessageEnd();
}

#endif // GAME_DLL