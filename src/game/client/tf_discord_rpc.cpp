#include "cbase.h"
#include "tf_discord_rpc.h"
#include <time.h>
#include "discord_rpc.h"

static bool g_bDiscordInitialized = false;
static time_t g_flDiscordStartTime = 0;

// Replace this with your actual Discord Application ID (string, from the Discord Developer Portal)
#define DISCORD_APP_ID "1525901849754996926"

void DiscordRPC_Init()
{
    if (g_bDiscordInitialized)
        return;

    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    // No handlers needed for simple, non-interactive rich presence.

    Discord_Initialize(DISCORD_APP_ID, &handlers, 1, NULL);

    g_flDiscordStartTime = time(NULL);
    g_bDiscordInitialized = true;

    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(presence));

    presence.details = "SOURCE SURGE";
    presence.state = "sv_airaccelerate 70";
    presence.startTimestamp = g_flDiscordStartTime;
    presence.largeImageKey = "logo"; // must match an asset key you upload on the Discord Dev Portal

    Discord_UpdatePresence(&presence);
}

void DiscordRPC_RunCallbacks()
{
    if (!g_bDiscordInitialized)
        return;

    // Keeps the IPC pipe to the Discord client alive. Cheap, safe to call every frame.
    Discord_RunCallbacks();
}

void DiscordRPC_Shutdown()
{
    if (!g_bDiscordInitialized)
        return;

    Discord_Shutdown();
    g_bDiscordInitialized = false;
}