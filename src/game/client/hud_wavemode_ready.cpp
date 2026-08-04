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

CHudWaveModeReady::CHudWaveModeReady( const char *pElementName )
	: CHudElement( pElementName ), BaseClass( NULL, "HudWaveModeReady" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_MISCSTATUS );

	m_pTitleLabel = new vgui::Label( this, "WaveModeReady_Title", "WAVEMODE" );
	m_pDifficultyLabel = new vgui::Label( this, "WaveModeReady_Difficulty", "" );
	m_pReadyButton = new vgui::Button( this, "WaveModeReady_ReadyBtn", "READY UP", this, "wavemode_ready" );

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

	vgui::HFont hFontTitle = pScheme->GetFont("HudFontSmallBold", true);
	vgui::HFont hFontRow = pScheme->GetFont("HudFontVerySmall", true);

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

	m_pReadyButton->SetFont( hFontTitle );
}

void CHudWaveModeReady::PerformLayout()
{
	BaseClass::PerformLayout();

	int screenWide, screenTall;
	vgui::surface()->GetScreenSize( screenWide, screenTall );

	int panelWide = 420;
	int panelTall = 460;
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

	m_pReadyButton->SetBounds( ( panelWide - 160 ) / 2, panelTall - 50, 160, 36 );
}

bool CHudWaveModeReady::ShouldDraw()
{
	if ( !TFGameRules() || !TFGameRules()->WaveMode_IsActive() || TFGameRules()->Wave2_IsActiveForHUD() )
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
		m_pDifficultyLabel->SetText("Press Ready when your team is set");
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