#include "cbase.h"

#ifdef GAME_DLL

#include "tf_gamemode_1v1.h"
#include "tf_player.h"
#include "gamerules.h"
#include "tf_player_stats.h"

ConVar sm_1v1_enabled("sm_1v1_enabled", "0", FCVAR_GAMEDLL | FCVAR_NOTIFY, "Toggles 1v1 Mode active state.");
ConVar sm_1v1_score_limit("sm_1v1_score_limit", "20", FCVAR_GAMEDLL | FCVAR_NOTIFY, "Score limit required to win 1v1 mode.");

C1v1Gamemode g_1v1Gamemode;

C1v1Gamemode::C1v1Gamemode()
{
    m_bIsActive = false;
    m_bWaitingForPlayers = false;
    m_bInCountdown = false;
    m_bMatchInProgress = false;
    m_flCountdownEndTime = 0.0f;
    m_iLastAnnouncedSecond = -1;
    m_iScoreLimit = 20;
}

int C1v1Gamemode::GetActivePlayerCount()
{
    int iCount = 0;
    for (int i = 1; i <= gpGlobals->maxClients; i++)
    {
        CTFPlayer* pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));
        if (pPlayer && pPlayer->IsConnected() && (pPlayer->GetTeamNumber() == TF_TEAM_RED || pPlayer->GetTeamNumber() == TF_TEAM_BLUE))
        {
            iCount++;
        }
    }
    return iCount;
}

void C1v1Gamemode::StartMode(int iScoreLimit)
{
    m_bIsActive = true;
    m_iScoreLimit = (iScoreLimit > 0) ? iScoreLimit : 20;

    sm_1v1_enabled.SetValue(1);
    sm_1v1_score_limit.SetValue(m_iScoreLimit);

    // Apply ConVars
    ConVarRef sv_force_spy_mode("sv_force_spy_mode");
    if (sv_force_spy_mode.IsValid()) sv_force_spy_mode.SetValue(1);

    ConVarRef sv_start_1v1("sv_start_1v1");
    if (sv_start_1v1.IsValid()) sv_start_1v1.SetValue(1);

    ConVarRef mp_timelimit("mp_timelimit");
    if (mp_timelimit.IsValid()) mp_timelimit.SetValue(0);

    ConVarRef mp_stalemate_enable("mp_stalemate_enable");
    if (mp_stalemate_enable.IsValid()) mp_stalemate_enable.SetValue(0);

    ConVarRef mp_respawnwavetime("mp_respawnwavetime");
    if (mp_respawnwavetime.IsValid()) mp_respawnwavetime.SetValue(0);

    ConVarRef mp_disable_respawn_times("mp_disable_respawn_times");
    if (mp_disable_respawn_times.IsValid()) mp_disable_respawn_times.SetValue(1);

    ResetPlayerScores();

    // Check player count on startup
    if (GetActivePlayerCount() < 2)
    {
        m_bWaitingForPlayers = true;
        m_bInCountdown = false;
        m_bMatchInProgress = false;
        UTIL_ClientPrintAll(HUD_PRINTCENTER, "1v1 MODE INITIALIZED\nWaiting for a second player...");
    }
    else
    {
        StartCountdown();
    }
}

void C1v1Gamemode::StartCountdown()
{
    m_bWaitingForPlayers = false;
    m_bInCountdown = true;
    m_bMatchInProgress = false;
    m_flCountdownEndTime = gpGlobals->curtime + 10.0f;
    m_iLastAnnouncedSecond = 10;

    UTIL_ClientPrintAll(HUD_PRINTCENTER, "MATCH STARTING IN 10 SECONDS\nPrepare yourself!");
}

void C1v1Gamemode::BeginMatch()
{
    m_bInCountdown = false;
    m_bMatchInProgress = true;

    ResetPlayerScores();

    char szMsg[128];
    Q_snprintf(szMsg, sizeof(szMsg), "1v1 MATCH STARTED!\nFirst to %d kills wins!", m_iScoreLimit);
    UTIL_ClientPrintAll(HUD_PRINTCENTER, szMsg);
}

void C1v1Gamemode::Update()
{
    if (!m_bIsActive)
        return;

    // Check if a player left mid-game/countdown
    int iPlayers = GetActivePlayerCount();
    if (iPlayers < 2 && !m_bWaitingForPlayers)
    {
        m_bWaitingForPlayers = true;
        m_bInCountdown = false;
        m_bMatchInProgress = false;
        UTIL_ClientPrintAll(HUD_PRINTCENTER, "Opponent left!\nWaiting for a second player...");
        return;
    }

    // Process 10-second countdown tick
    if (m_bInCountdown)
    {
        float flRemaining = m_flCountdownEndTime - gpGlobals->curtime;
        int iSecondsLeft = ceil(flRemaining);

        if (iSecondsLeft > 0 && iSecondsLeft != m_iLastAnnouncedSecond)
        {
            m_iLastAnnouncedSecond = iSecondsLeft;
            char szCountMsg[128];
            Q_snprintf(szCountMsg, sizeof(szCountMsg), "MATCH STARTING IN %d...", iSecondsLeft);
            UTIL_ClientPrintAll(HUD_PRINTCENTER, szCountMsg);
        }

        if (flRemaining <= 0.0f)
        {
            BeginMatch();
        }
    }
}

void C1v1Gamemode::StopMode()
{
    m_bIsActive = false;
    m_bWaitingForPlayers = false;
    m_bInCountdown = false;
    m_bMatchInProgress = false;

    sm_1v1_enabled.SetValue(0);

    ConVarRef sv_start_1v1("sv_start_1v1");
    if (sv_start_1v1.IsValid()) sv_start_1v1.SetValue(0);

    UTIL_ClientPrintAll(HUD_PRINTCENTER, "1v1 MODE ENDED");
}

void C1v1Gamemode::ResetPlayerScores()
{
    for (int i = 1; i <= gpGlobals->maxClients; i++)
    {
        CTFPlayer* pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));
        if (pPlayer)
        {
            pPlayer->ResetFragCount();
            pPlayer->ResetDeathCount();
        }
    }
}

void C1v1Gamemode::OnPlayerSpawn(CBasePlayer* pPlayer)
{
    if (!m_bIsActive || !pPlayer)
        return;

    CTFPlayer* pTFPlayer = ToTFPlayer(pPlayer);
    if (!pTFPlayer)
        return;

    // Full health & ammo reset on spawn
    pTFPlayer->SetHealth(pTFPlayer->GetMaxHealth());
    for (int i = 0; i < MAX_WEAPONS; i++)
    {
        CBaseCombatWeapon* pWeapon = pTFPlayer->GetWeapon(i);
        if (pWeapon)
        {
            int iPrimaryAmmoType = pWeapon->GetPrimaryAmmoType();
            if (iPrimaryAmmoType != -1)
                pTFPlayer->SetAmmoCount(pTFPlayer->GetMaxAmmo(iPrimaryAmmoType), iPrimaryAmmoType);

            if (pWeapon->UsesClipsForAmmo1())
                pWeapon->m_iClip1 = pWeapon->GetMaxClip1();
        }
    }

    // If we were waiting for players and a 2nd player just spawned in, trigger countdown!
    if (m_bWaitingForPlayers && GetActivePlayerCount() >= 2)
    {
        StartCountdown();
    }
}

void C1v1Gamemode::OnPlayerKilled(CBaseEntity* pVictim, CBaseEntity* pAttacker)
{
    if (!m_bIsActive || !pVictim)
        return;

    CTFPlayer* pTFVictim = ToTFPlayer(pVictim);
    CTFPlayer* pTFAttacker = ToTFPlayer(pAttacker);

    // Only count score if the match is actively running (not during 10s countdown)
    if (m_bMatchInProgress && pTFAttacker && pTFAttacker != pTFVictim)
    {
        int iAttackerScore = pTFAttacker->FragCount();

        pTFAttacker->m_iHealth = pTFAttacker->GetMaxHealth();
        for (int i = 0; i < MAX_WEAPONS; i++)
        {
            CBaseCombatWeapon* pWeapon = pTFAttacker->GetWeapon(i);
            if (pWeapon)
            {
                int iPrimaryAmmoType = pWeapon->GetPrimaryAmmoType();
                if (iPrimaryAmmoType != -1)
                    pTFAttacker->SetAmmoCount(pTFAttacker->GetMaxAmmo(iPrimaryAmmoType), iPrimaryAmmoType);

                if (pWeapon->UsesClipsForAmmo1())
                    pWeapon->m_iClip1 = pWeapon->GetMaxClip1();
            }
        }

        char szScoreMsg[128];
        Q_snprintf(szScoreMsg, sizeof(szScoreMsg), "%s scored a kill! [%d / %d]",
            pTFAttacker->GetPlayerName(), iAttackerScore, m_iScoreLimit);
        UTIL_ClientPrintAll(HUD_PRINTTALK, szScoreMsg);

        if (iAttackerScore >= m_iScoreLimit)
        {
            EndMatch(pTFAttacker);
            return;
        }
    }

    if (pTFVictim)
    {
        pTFVictim->ForceRespawn();
    }
}

void C1v1Gamemode::EndMatch(CBasePlayer* pWinner)
{
    if (pWinner)
    {
        g_PlayerStatsManager.AddOneVOneWin(pWinner); 
        char szWinCenter[128];
        Q_snprintf(szWinCenter, sizeof(szWinCenter), "WINNER!\n%s wins the 1v1 duel!", pWinner->GetPlayerName());
        UTIL_ClientPrintAll(HUD_PRINTCENTER, szWinCenter);
    }

    StopMode();
}

CON_COMMAND(start_1v1_mode, "Launches 1v1 mode. Usage: start_1v1_mode [score_limit]")
{
    int iLimit = 20;
    if (args.ArgC() > 1)
        iLimit = atoi(args[1]);

    g_1v1Gamemode.StartMode(iLimit);
}

CON_COMMAND(stop_1v1_mode, "Stops the 1v1 mode.")
{
    g_1v1Gamemode.StopMode();
}

#endif // GAME_DLL