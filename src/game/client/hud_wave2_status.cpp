#include "cbase.h"
#include "hud_wave2_status.h"
#include "iclientmode.h"
#include "tf_gamerules.h"
#include <vgui/ISurface.h>

using namespace vgui;

DECLARE_HUDELEMENT( CHudWave2Status );

CHudWave2Status::CHudWave2Status( const char *pElementName )
	: CHudElement( pElementName ), BaseClass( NULL, "HudWave2Status" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_MISCSTATUS );

	m_pWaveLabel = new vgui::Label( this, "Wave2_WaveLabel", "" );
	m_pEnemiesLabel = new vgui::Label( this, "Wave2_EnemiesLabel", "" );
	m_pCooldownLabel = new vgui::Label( this, "Wave2_CooldownLabel", "" );
	m_pBuffLabel = new vgui::Label( this, "Wave2_BuffLabel", "" );
}

void CHudWave2Status::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetPaintBackgroundEnabled( false );
	SetPaintBorderEnabled( false );

	vgui::HFont hFontBig = pScheme->GetFont( "HudFontMediumBold", true );
	vgui::HFont hFontSmall = pScheme->GetFont( "HudFontSmallBold", true );

	m_pWaveLabel->SetFont( hFontBig );
	m_pWaveLabel->SetFgColor( Color( 235, 226, 202, 255 ) );
	m_pWaveLabel->SetContentAlignment( vgui::Label::a_center );
	m_pWaveLabel->SetPaintBackgroundEnabled( false );

	m_pEnemiesLabel->SetFont( hFontSmall );
	m_pEnemiesLabel->SetFgColor( Color( 200, 190, 170, 255 ) );
	m_pEnemiesLabel->SetContentAlignment( vgui::Label::a_center );
	m_pEnemiesLabel->SetPaintBackgroundEnabled( false );

	m_pCooldownLabel->SetFont( hFontBig );
	m_pCooldownLabel->SetFgColor( Color( 205, 149, 12, 255 ) ); // TF2 orange
	m_pCooldownLabel->SetContentAlignment( vgui::Label::a_center );
	m_pCooldownLabel->SetPaintBackgroundEnabled( false );

	m_pBuffLabel->SetFont( hFontSmall );
	m_pBuffLabel->SetFgColor( Color( 120, 200, 120, 255 ) );
	m_pBuffLabel->SetContentAlignment( vgui::Label::a_center );
	m_pBuffLabel->SetPaintBackgroundEnabled( false );
}

void CHudWave2Status::PerformLayout()
{
	BaseClass::PerformLayout();

	int screenWide, screenTall;
	vgui::surface()->GetScreenSize( screenWide, screenTall );

	int panelWide = 400;
	int panelTall = 100;
	SetPos( ( screenWide - panelWide ) / 2, 20 ); // top-center
	SetSize( panelWide, panelTall );

	m_pWaveLabel->SetBounds( 0, 0, panelWide, 24 );
	m_pEnemiesLabel->SetBounds( 0, 26, panelWide, 20 );
	m_pCooldownLabel->SetBounds( 0, 48, panelWide, 26 );
	m_pBuffLabel->SetBounds( 0, 76, panelWide, 20 );
}

bool CHudWave2Status::ShouldDraw()
{
	if ( !TFGameRules() || !TFGameRules()->Wave2_IsActiveForHUD())
		return false;

	return CHudElement::ShouldDraw();
}

void CHudWave2Status::OnTick()
{
	if ( !TFGameRules() )
		return;

	char buf[128];

	V_snprintf( buf, sizeof(buf), "WAVE %d", TFGameRules()->Wave2_GetCurrentWaveForHUD() );
	m_pWaveLabel->SetText( buf );

	V_snprintf( buf, sizeof(buf), "Enemies Remaining: %d", TFGameRules()->Wave2_GetBotsAliveForHUD() );
	m_pEnemiesLabel->SetText( buf );

	if ( TFGameRules()->Wave2_IsInCooldownForHUD() )
	{
		float flTimeLeft = TFGameRules()->Wave2_GetCooldownEndTimeForHUD() - gpGlobals->curtime;
		if ( flTimeLeft < 0.0f )
			flTimeLeft = 0.0f;

		V_snprintf( buf, sizeof(buf), "Next wave in %.1f...", flTimeLeft );
		m_pCooldownLabel->SetText( buf );
		m_pCooldownLabel->SetVisible( true );

		int nBuff = TFGameRules()->Wave2_GetLastBuffTypeForHUD();
		const char *pszBuffText = "";
		switch ( nBuff )
		{
			case 0: pszBuffText = "Enemies gained MORE HEALTH"; break;
			case 1: pszBuffText = "Enemies gained MORE DAMAGE"; break;
			case 2: pszBuffText = "Enemies gained MORE RESISTANCE"; break;
			default: pszBuffText = ""; break;
		}
		m_pBuffLabel->SetText( pszBuffText );
		m_pBuffLabel->SetVisible( nBuff >= 0 );
	}
	else
	{
		m_pCooldownLabel->SetVisible( false );
		m_pBuffLabel->SetVisible( false );
	}
}