#include "cbase.h"
#include "tf_playerstats_client.h"
#include "usermessages.h"

static int g_iStats_Seconds = 0;
static int g_iStats_Wins = 0;
static int g_iStats_Kills = 0;

int GetClientStat_SecondsPlayed() { return g_iStats_Seconds; }
int GetClientStat_OneVOneWins()   { return g_iStats_Wins; }
int GetClientStat_TotalKills()    { return g_iStats_Kills; }

USER_MESSAGE( PlayerStats )
{
	g_iStats_Seconds = msg.ReadLong();
	g_iStats_Wins    = msg.ReadLong();
	g_iStats_Kills   = msg.ReadLong();
}