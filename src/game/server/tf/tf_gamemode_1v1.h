#ifndef TF_GAMEMODE_1V1_H
#define TF_GAMEMODE_1V1_H

#ifdef GAME_DLL

class C1v1Gamemode
{
public:
    C1v1Gamemode();

    void StartMode(int iScoreLimit = 20);
    void StopMode();
    void StartCountdown();
    void BeginMatch();
    void EndMatch(CBasePlayer* pWinner); // <-- AJOUTÉ ICI !
    void Update();

    void OnPlayerKilled(CBaseEntity* pVictim, CBaseEntity* pAttacker);
    void OnPlayerSpawn(CBasePlayer* pPlayer);

    int  GetActivePlayerCount();
    bool IsActive() const { return m_bIsActive; }
    bool IsInCountdown() const { return m_bInCountdown; }

private:
    void ResetPlayerScores();

    bool  m_bIsActive;
    bool  m_bWaitingForPlayers;
    bool  m_bInCountdown;
    bool  m_bMatchInProgress;
    float m_flCountdownEndTime;
    int   m_iLastAnnouncedSecond;
    int   m_iScoreLimit;
};

extern C1v1Gamemode g_1v1Gamemode;

#endif // GAME_DLL
#endif // TF_GAMEMODE_1V1_H