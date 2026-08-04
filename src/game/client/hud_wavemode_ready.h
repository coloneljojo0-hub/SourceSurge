#ifndef HUD_WAVEMODE_READY_H
#define HUD_WAVEMODE_READY_H

#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include "hud.h"
#include "hudelement.h"
#include <inputsystem/ButtonCode.h>

class CHudWaveModeReady : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudWaveModeReady, vgui::Panel );

public:
	CHudWaveModeReady( const char *pElementName );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void PerformLayout();
	virtual bool ShouldDraw();
	virtual void OnThink();
	virtual void OnCommand(const char* command);
	bool ToggleReady(ButtonCode_t code);

private:
	vgui::Label		*m_pTitleLabel;
	vgui::Label		*m_pDifficultyLabel;
	vgui::Label		*m_pPlayerRows[MAX_PLAYERS];
	vgui::Button	*m_pReadyButton;

	void UpdatePlayerRows();
};

#endif // HUD_WAVEMODE_READY_H