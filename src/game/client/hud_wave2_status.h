#ifndef HUD_WAVE2_STATUS_H
#define HUD_WAVE2_STATUS_H
#ifdef _WIN32
#pragma once
#endif

#include "hud.h"
#include "hudelement.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/Label.h>

class CHudWave2Status : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHudWave2Status, vgui::EditablePanel );
	
	float m_flBuffRollEndTime;
	float m_flNextBuffRollTime;
	bool m_bBuffRolling;
	int m_iCurrentRollBuff;
	int m_iFinalBuff;

public:
	CHudWave2Status( const char *pElementName );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void PerformLayout();
	virtual void OnThink();
	virtual bool ShouldDraw();

private:
	vgui::Label *m_pWaveLabel;
	vgui::Label *m_pEnemiesLabel;
	vgui::Label *m_pCooldownLabel;
	vgui::Label *m_pBuffLabel;
};

#endif // HUD_WAVE2_STATUS_H