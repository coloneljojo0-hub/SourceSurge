#pragma once

#ifndef TF_HUD_MODMENU_H
#define TF_HUD_MODMENU_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Slider.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Panel.h>

class CTFModMenu : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CTFModMenu, vgui::Frame);

public:
	CTFModMenu(vgui::Panel* parent);
	virtual ~CTFModMenu();

	virtual void OnCommand(const char* command) OVERRIDE;
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) OVERRIDE;
	virtual void PerformLayout() OVERRIDE;
	MESSAGE_FUNC(OnSliderMoved, "SliderMoved");

	void SwitchTab(int tabIndex);
	void SaveSettings();
	void LoadSettings();

public:
	// --- SPEEDOMETER CONTROLS ---
	vgui::Label* m_pSpeedometerHeader;
	vgui::CheckButton* m_pSpeedometerCheck;
	vgui::ComboBox* m_pUnitCombo;
	vgui::ComboBox* m_pColorCombo;
	vgui::TextEntry* m_pSizeEntry;

	vgui::Slider* m_pXSlider;
	vgui::TextEntry* m_pXEntry;
	vgui::Slider* m_pYSlider;
	vgui::TextEntry* m_pYEntry;

	vgui::Label* m_pUnitLabel;
	vgui::Label* m_pColorLabel;
	vgui::Label* m_pSizeLabel;
	vgui::Label* m_pXLabel;
	vgui::Label* m_pYLabel;

	// --- FPS CONTROLS ---
	vgui::Label* m_pFPSHeader;
	vgui::CheckButton* m_pFPSCheck;
	vgui::ComboBox* m_pFPSColorCombo;
	vgui::TextEntry* m_pFPSSizeEntry;

	vgui::Slider* m_pFPSXSlider;
	vgui::TextEntry* m_pFPSXEntry;
	vgui::Slider* m_pFPSYSlider;
	vgui::TextEntry* m_pFPSYEntry;

	vgui::Label* m_pFPSColorLabel;
	vgui::Label* m_pFPSSizeLabel;
	vgui::Label* m_pFPSXLabel;
	vgui::Label* m_pFPSYLabel;

	// --- STATS ---
	vgui::Button* m_pTabStatsBtn;
	vgui::Panel* m_pStatsPage;
	vgui::Label* m_pStatsTimeLabel;
	vgui::Label* m_pStatsWinsLabel;
	vgui::Label* m_pStatsKillsLabel;

	virtual void OnThink() OVERRIDE;
	void UpdateStatsDisplay();

	// --- TABS & PAGES ---
	vgui::Button* m_pTabMovementBtn;
	vgui::Button* m_pTabHudBtn;

	vgui::Panel* m_pMovementPage;
	vgui::Panel* m_pHudPage;

	int m_iActiveTab;
};

void ToggleModMenu();

#endif // TF_HUD_MODMENU_H