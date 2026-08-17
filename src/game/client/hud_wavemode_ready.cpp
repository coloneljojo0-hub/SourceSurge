#include "cbase.h"
#include "hud_wavemode_ready.h"
#include "iclientmode.h"
#include "tf_gamerules.h"
#include "c_tf_player.h"
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>

using namespace vgui;

DECLARE_HUDELEMENT( CHudWaveModeReady );

bool WaveModeHudElementKeyInput(int down, ButtonCode_t keynum, const char* pszCurrentBinding)
{
	CHudWaveModeReady* pPanel = (CHudWaveModeReady*)GET_HUDELEMENT(CHudWaveModeReady);
	if (pPanel && down == 1)
	{
		return pPanel->ToggleReady(keynum);
	}
	return false;
}

// ============================================================================

CHudWaveModeReady::CHudWaveModeReady( const char *pElementName )
	: CHudElement( pElementName ), BaseClass( NULL, "HudWaveModeReady" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_MISCSTATUS );

	m_pTitleLabel = new vgui::Label( this, "WaveModeReady_Title", "WAVEMODE" );
	m_pDifficultyLabel = new vgui::Label( this, "WaveModeReady_Difficulty", "" );
	m_pReadyButton = new vgui::Button( this, "WaveModeReady_ReadyBtn", "ready up - F4", this, "wavemode_ready" );

	for ( int i = 0; i < MAX_PLAYERS; i++ )
	{
		char szName[32];
		V_snprintf( szName, sizeof(szName), "WaveModeReady_Row%d", i );
		m_pPlayerRows[i] = new vgui::Label( this, szName, "" );
		m_pPlayerRows[i]->SetVisible( false );
	}

	SetKeyBoardInputEnabled( false );
	SetMouseInputEnabled( true );
}

void CHudWaveModeReady::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetPaintBackgroundEnabled( true );
	SetBgColor( Color( 20, 20, 20, 210 ) );
	SetPaintBorderEnabled( false );

	vgui::HFont hFontTitle = pScheme->GetFont("HudFontMediumBold", true);
	vgui::HFont hFontRow = pScheme->GetFont("HudFontSmallBold", true);

	m_pTitleLabel->SetFont( hFontTitle );
	m_pTitleLabel->SetFgColor( Color( 235, 226, 202, 255 ) );
	m_pTitleLabel->SetContentAlignment( vgui::Label::a_center );
	m_pTitleLabel->SetPaintBackgroundEnabled( false );

	m_pDifficultyLabel->SetFont( hFontRow );
	m_pDifficultyLabel->SetFgColor( Color( 205, 149, 12, 255 ) );
	m_pDifficultyLabel->SetContentAlignment( vgui::Label::a_center );
	m_pDifficultyLabel->SetPaintBackgroundEnabled( false );

	for ( int i = 0; i < MAX_PLAYERS; i++ )
	{
		m_pPlayerRows[i]->SetFont( hFontRow );
		m_pPlayerRows[i]->SetContentAlignment( vgui::Label::a_west );
		m_pPlayerRows[i]->SetPaintBackgroundEnabled( false );
	}

	m_pReadyButton->SetFont( hFontRow );
}

void CHudWaveModeReady::PerformLayout()
{
	BaseClass::PerformLayout();

	int screenWide, screenTall;
	vgui::surface()->GetScreenSize( screenWide, screenTall );

	int panelWide = 460;
	int panelTall = 500;
	SetPos( ( screenWide - panelWide ) / 2, ( screenTall - panelTall ) / 2 );
	SetSize( panelWide, panelTall );

	m_pTitleLabel->SetBounds( 0, 10, panelWide, 24 );
	m_pDifficultyLabel->SetBounds( 0, 36, panelWide, 20 );

	int rowY = 66;
	for ( int i = 0; i < MAX_PLAYERS; i++ )
	{
		if ( m_pPlayerRows[i]->IsVisible() )
		{
			m_pPlayerRows[i]->SetBounds( 20, rowY, panelWide - 40, 20 );
			rowY += 24;
		}
	}

	m_pReadyButton->SetBounds( ( panelWide - 200 ) / 2, panelTall - 50, 200, 36 );
}

bool CHudWaveModeReady::ShouldDraw()
{
	if (!TFGameRules() || !TFGameRules()->WaveMode_IsActive() || TFGameRules()->Wave2_IsActiveForHUD())
		return false;

	if (TFGameRules()->WaveMode_IsGameOver())
		return false;

	return CHudElement::ShouldDraw();
}

void CHudWaveModeReady::OnThink()
{
	if (!TFGameRules())
		return;

	float flCountdownEnd = TFGameRules()->WaveMode_GetCountdownEndTimeForHUD();

	if (flCountdownEnd >= 0.0f)
	{
		float flTimeLeft = flCountdownEnd - gpGlobals->curtime;
		int nSeconds = (int)ceil(flTimeLeft);

		char szCountdown[128];
		if (nSeconds > 0)
		{
			V_snprintf(szCountdown, sizeof(szCountdown), "EVERYONE READY - STARTING IN %d...", nSeconds);
		}
		else
		{
			V_snprintf(szCountdown, sizeof(szCountdown), "GOOD LUCK!");
		}

		m_pDifficultyLabel->SetText(szCountdown);
		m_pDifficultyLabel->SetFgColor(Color(120, 200, 120, 255));
	}
	else
	{
		m_pDifficultyLabel->SetText( "Press Ready when your team is set" );
		m_pDifficultyLabel->SetFgColor(Color(205, 149, 12, 255));
	}

	UpdatePlayerRows();
}

void CHudWaveModeReady::UpdatePlayerRows()
{
	int nRowIndex = 0;

	for ( int i = 1; i <= MAX_PLAYERS; i++ )
	{
		CBasePlayer* pPlayer = UTIL_PlayerByIndex(i);
		if (!pPlayer)
			continue;

		C_TFPlayer* pTFPlayer = ToTFPlayer(pPlayer);
		if ( !pTFPlayer || pTFPlayer->GetTeamNumber() != TF_TEAM_RED )
			continue;

		if ( nRowIndex >= MAX_PLAYERS )
			break;

		bool bReady = TFGameRules()->WaveMode_IsPlayerReady( i );

		char szRow[64];
		V_snprintf( szRow, sizeof(szRow), "%s  %s", bReady ? "[READY]" : "[NOT READY]", pPlayer->GetPlayerName() );

		m_pPlayerRows[nRowIndex]->SetText( szRow );
		m_pPlayerRows[nRowIndex]->SetFgColor( bReady ? Color( 120, 200, 120, 255 ) : Color( 200, 80, 80, 255 ) );
		m_pPlayerRows[nRowIndex]->SetVisible( true );

		nRowIndex++;
	}

	// hide any leftover rows from a previous frame with more players
	for ( int i = nRowIndex; i < MAX_PLAYERS; i++ )
	{
		m_pPlayerRows[i]->SetVisible( false );
	}

	InvalidateLayout();
}

void CHudWaveModeReady::OnCommand( const char *command )
{
	if ( !Q_stricmp( command, "wavemode_ready" ) )
	{
		engine->ClientCmd_Unrestricted( "tf_wavemode_ready\n" );
		return;
	}

	BaseClass::OnCommand( command );
}

bool CHudWaveModeReady::ToggleReady(ButtonCode_t code)
{
	if (!IsVisible())
		return false;

	if (code == KEY_F4)
	{
		engine->ClientCmd_Unrestricted("tf_wavemode_ready\n");
		return true;
	}

	return false;
}

// ============================================================================
//  WAVEMODE GAME OVER PANEL
// ============================================================================

class CHudWaveModeGameOver : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CHudWaveModeGameOver, vgui::Panel);

public:
	CHudWaveModeGameOver(const char* pElementName)
		: CHudElement(pElementName), vgui::Panel(NULL, "HudWaveModeGameOver")
	{
		vgui::Panel* pParent = g_pClientMode->GetViewport();
		SetParent(pParent);

		SetHiddenBits(HIDEHUD_MISCSTATUS);

		m_pStatsLabel = new vgui::Label(this, "WaveModeGameOver_Stats", "");
		m_pRetryButton = new vgui::Button(this, "WaveModeGameOver_Retry", "retry - F3", this, "wavemode_retry");
		m_pMenuButton = new vgui::Button(this, "WaveModeGameOver_Menu", "quit - F4", this, "wavemode_menu");

		SetKeyBoardInputEnabled(false);
		SetMouseInputEnabled(false);
	}

	virtual void ApplySchemeSettings(vgui::IScheme* pScheme);
	virtual void PerformLayout();
	virtual bool ShouldDraw();
	virtual void OnKeyCodePressed(vgui::KeyCode code);
	virtual void OnCommand(const char* command);

	void UpdateStats();

private:
	vgui::Label* m_pStatsLabel;
	vgui::Button* m_pRetryButton;
	vgui::Button* m_pMenuButton;
};

DECLARE_HUDELEMENT(CHudWaveModeGameOver);

// ============================================================================
// CHudWaveModeGameOver implementation
// ============================================================================

void CHudWaveModeGameOver::ApplySchemeSettings(vgui::IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetPaintBackgroundEnabled(true);
	SetBgColor(Color(20, 20, 20, 210));
	SetPaintBorderEnabled(false);

	vgui::HFont hFontRow = pScheme->GetFont("HudFontSmallBold", true);

	m_pStatsLabel->SetFont(hFontRow);
	m_pStatsLabel->SetFgColor(Color(235, 226, 202, 255));
	m_pStatsLabel->SetContentAlignment(vgui::Label::a_center);
	m_pStatsLabel->SetPaintBackgroundEnabled(false);

	// Use the small row font for the button labels so they're more compact
	m_pRetryButton->SetFont(hFontRow);
	m_pMenuButton->SetFont(hFontRow);
}

void CHudWaveModeGameOver::PerformLayout()
{
	BaseClass::PerformLayout();

	int screenWide, screenTall;
	vgui::surface()->GetScreenSize(screenWide, screenTall);

	int panelWide = 460;
	int panelTall = 260;
	SetPos((screenWide - panelWide) / 2, (screenTall - panelTall) / 2);
	SetSize(panelWide, panelTall);

	m_pStatsLabel->SetBounds(0, 20, panelWide, 120);
	m_pRetryButton->SetBounds(40, panelTall - 60, 160, 36);
	m_pMenuButton->SetBounds(panelWide - 200, panelTall - 60, 160, 36);
}

bool CHudWaveModeGameOver::ShouldDraw()
{
	if (!TFGameRules() || !TFGameRules()->WaveMode_IsGameOver())
		return false;

	UpdateStats();
	return CHudElement::ShouldDraw();
}

void CHudWaveModeGameOver::UpdateStats()
{
	char buf[256];
	const char* pszDiff = "Unknown";
	switch ( TFGameRules()->WaveMode_GetDifficultyForHUD() )
	{
		case 0: pszDiff = "Hard"; break;
		case 1: pszDiff = "Harder"; break;
		case 2: pszDiff = "Hardest"; break;
		case 3: pszDiff = "No-Hit"; break;
	}
	V_snprintf(buf, sizeof(buf),
		"GAME OVER!\n\nKills: %d\nDifficulty: %s",
		TFGameRules()->WaveMode_GetKillsForHUD(),
		pszDiff);

	m_pStatsLabel->SetText(buf);
}

void CHudWaveModeGameOver::OnCommand(const char* command)
{
	if (!Q_stricmp(command, "wavemode_retry"))
	{
		// Server tf_wavemode_retry resets wave state and sets tf_wavemode_pending_diff.
		// After the map reload, player_spawn will fire Wave2_Start automatically.
		engine->ClientCmd_Unrestricted("tf_wavemode_retry\n");
		SetVisible(false);
		return;
	}
	else if (!Q_stricmp(command, "wavemode_menu"))
	{
		engine->ClientCmd_Unrestricted("disconnect\n");
		return;
	}

	BaseClass::OnCommand(command);
}

void CHudWaveModeGameOver::OnKeyCodePressed(vgui::KeyCode code)
{
	if (code == KEY_F3)
	{
		// Get current map name (strip maps/ prefix and .bsp suffix).
		char szMapName[64] = "surge_sandbox";
		const char *pszFullName = engine->GetLevelName();
		if (pszFullName && pszFullName[0])
		{
			// V_FileBase extracts just "surge_sandbox" from any path like
			// "D:/.mygame/.../maps/surge_sandbox.bsp" or "maps/surge_sandbox.bsp"
			V_FileBase(pszFullName, szMapName, sizeof(szMapName));
		}

		// tf_wavemode_retry (server): saves diff, resets state, kicks bots.
		// map: triggers full reload — player_spawn fires Wave2_Start automatically.
		char szCmd[256];
		Q_snprintf(szCmd, sizeof(szCmd),
			"tf_wavemode_retry; map %s;", szMapName);
		engine->ClientCmd_Unrestricted(szCmd);
		SetVisible(false);
	}
	else if (code == KEY_F4)
	{
		engine->ClientCmd_Unrestricted("disconnect\n");
		SetVisible(false);
	}
}

bool WaveModeGameOverHudElementKeyInput(int down, ButtonCode_t keynum, const char* pszCurrentBinding)
{
	if (down != 1)
		return false;

	CHudWaveModeGameOver* pPanel = (CHudWaveModeGameOver*)GET_HUDELEMENT(CHudWaveModeGameOver);
	if (!pPanel)
		return false;

	if (keynum == KEY_F3)
	{
		pPanel->OnKeyCodePressed(keynum);
		return true; // intercept — don't let game take screenshot
	}

	if (keynum == KEY_F4)
	{
		pPanel->OnKeyCodePressed(keynum);
		return false; // pass through — let game take screenshot
	}

	return false;
}
