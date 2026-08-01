#pragma once

#ifndef TF_HUD_PLAYMENU_H
#define TF_HUD_PLAYMENU_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/CheckButton.h>

class CTFPlayMenu : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CTFPlayMenu, vgui::Frame);

public:
	CTFPlayMenu(vgui::Panel* parent);
	virtual ~CTFPlayMenu();

	virtual void OnCommand(const char* command) OVERRIDE;
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) OVERRIDE;
	virtual void PerformLayout() OVERRIDE;
	virtual void PaintBackground() OVERRIDE;

	void SwitchTab(int mainTab, int subTab = 0);

public:
	// --- Main Navigation Tabs ---
	vgui::Button* m_pTabSingleplayerBtn;
	vgui::Button* m_pTabMultiplayerBtn;

	// --- Sub-Navigation Tabs (Multiplayer) ---
	vgui::Button* m_pSubTabCoopBtn;
	vgui::Button* m_pSubTab1v1Btn;

	// --- Primary Pages ---
	vgui::Panel* m_pSingleplayerPage;
	vgui::Panel* m_pCoopPage;
	vgui::Panel* m_p1v1Page;

	// --- Singleplayer Page Elements ---
	vgui::Label* m_pSPMapLabel;
	vgui::ComboBox* m_pSPMapCombo;
	vgui::Label* m_pSPDiffLabel;
	vgui::ComboBox* m_pSPDifficultyCombo;
	vgui::Button* m_pSPPlayButton;

	// --- Co-op Page Elements ---
	vgui::Label* m_pCoopMapLabel;
	vgui::ComboBox* m_pCoopMapCombo;
	vgui::Label* m_pCoopDiffLabel;
	vgui::ComboBox* m_pCoopDiffCombo;
	vgui::Label* m_pCoopIPLabel;
	vgui::TextEntry* m_pCoopIPEntry;
	vgui::Button* m_pCoopHostBtn;
	vgui::Label* m_pCoopJoinLabel;
	vgui::TextEntry* m_pCoopJoinIPEntry;
	vgui::Button* m_pCoopJoinBtn;

	// --- 1v1 Page Elements ---
	vgui::Label* m_p1v1MapLabel;
	vgui::ComboBox* m_p1v1MapCombo;
	vgui::Label* m_p1v1RoundsLabel;
	vgui::TextEntry* m_p1v1RoundsEntry;
	vgui::CheckButton* m_p1v1NoHookCheck;
	vgui::CheckButton* m_p1v1NoSapperCheck;
	vgui::CheckButton* m_p1v1NoKnifeCheck;
	vgui::Label* m_p1v1IPLabel;
	vgui::TextEntry* m_p1v1IPEntry;
	vgui::Button* m_p1v1HostBtn;
	vgui::Label* m_p1v1JoinLabel;
	vgui::TextEntry* m_p1v1JoinIPEntry;
	vgui::Button* m_p1v1JoinBtn;

	int m_iActiveMainTab;
	int m_iActiveSubTab;
};

void ToggleplayMenu();

#endif // TF_HUD_PLAYMENU_H