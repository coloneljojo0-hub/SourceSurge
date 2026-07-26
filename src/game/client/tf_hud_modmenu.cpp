#include "cbase.h"
#include "tf_hud_modmenu.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui_controls/Button.h>
#include "vgui_controls/Panel.h"
#include "iclientmode.h"
#include <vgui/IScheme.h>
#include <vgui_controls/Controls.h>
#include "tf_playerstats_client.h"

using namespace vgui;

static CTFModMenu* g_pModMenu = NULL;

extern ConVar cl_speedometer_enabled;
extern ConVar cl_speedometer_unit;
extern ConVar cl_speedometer_color;
extern ConVar cl_speedometer_size;
extern ConVar cl_speedometer_x;
extern ConVar cl_speedometer_y;

extern ConVar cl_fps_enabled;
extern ConVar cl_fps_color;
extern ConVar cl_fps_size;
extern ConVar cl_fps_x;
extern ConVar cl_fps_y;

CTFModMenu::CTFModMenu(vgui::Panel* parent) : BaseClass(parent, "TFModMenu")
{
	HScheme scheme = vgui::scheme()->LoadSchemeFromFile("resource/ClientScheme.res", "ClientScheme");
	SetScheme(scheme);

	SetTitle("Mod Menu", false);
	SetMoveable(true);
	SetSizeable(true);
	SetVisible(false);
	SetProportional(false);
	SetMinimumSize(380, 520);

	const int panelWide = 380;
	const int panelTall = 520;
	SetSize(panelWide, panelTall);

	int screenWide, screenTall;
	surface()->GetScreenSize(screenWide, screenTall);

	// --- Tab buttons ---
	m_pTabMovementBtn = new Button(this, "TabMovementBtn", "Movement", this, "tab_movement");
	m_pTabMovementBtn->SetPos(10, 30);
	m_pTabMovementBtn->SetSize(170, 24);

	m_pTabHudBtn = new Button(this, "TabHudBtn", "HUD", this, "tab_hud");
	m_pTabHudBtn->SetPos(190, 30);
	m_pTabHudBtn->SetSize(170, 24);

	m_pTabStatsBtn = new Button(this, "TabStatsBtn", "Stats", this, "tab_stats");
	m_pTabStatsBtn->SetPos(250, 30);
	m_pTabStatsBtn->SetSize(115, 24);

	// --- Pages ---
	m_pMovementPage = new Panel(this, "MovementPage");
	m_pMovementPage->SetPos(10, 65);
	m_pMovementPage->SetSize(360, 440);

	m_pHudPage = new Panel(this, "HudPage");
	m_pHudPage->SetPos(10, 65);
	m_pHudPage->SetSize(360, 440);

	m_pStatsPage = new Panel(this, "StatsPage");
	m_pStatsPage->SetPos(10, 65);
	m_pStatsPage->SetSize(360, 440);

	m_pStatsTimeLabel = new Label(m_pStatsPage, "StatsTimeLabel", "Time Played: --:--:--");
	m_pStatsTimeLabel->SetPos(10, 10);
	m_pStatsTimeLabel->SetSize(340, 24);

	m_pStatsWinsLabel = new Label(m_pStatsPage, "StatsWinsLabel", "1v1 Wins: -");
	m_pStatsWinsLabel->SetPos(10, 40);
	m_pStatsWinsLabel->SetSize(340, 24);

	m_pStatsKillsLabel = new Label(m_pStatsPage, "StatsKillsLabel", "Total Kills: -");
	m_pStatsKillsLabel->SetPos(10, 70);
	m_pStatsKillsLabel->SetSize(340, 24);

	// ==========================================
	// ----------- SPEEDOMETER SECTION -----------
	// ==========================================
	m_pSpeedometerHeader = new Label(m_pHudPage, "SpeedoHeader", "---- Speedometer ----");
	m_pSpeedometerHeader->SetPos(10, 5);
	m_pSpeedometerHeader->SetSize(340, 20);

	m_pSpeedometerCheck = new CheckButton(m_pHudPage, "SpeedometerCheck", "Enable Speedometer");
	m_pSpeedometerCheck->SetPos(10, 28);
	m_pSpeedometerCheck->SetSize(280, 24);

	m_pUnitLabel = new Label(m_pHudPage, "UnitLabel", "Unit:");
	m_pUnitLabel->SetPos(10, 55);
	m_pUnitLabel->SetSize(80, 24);

	m_pUnitCombo = new ComboBox(m_pHudPage, "UnitCombo", 4, false);
	m_pUnitCombo->SetPos(100, 55);
	m_pUnitCombo->SetSize(140, 24);
	m_pUnitCombo->AddItem("u/s (Units)", NULL);
	m_pUnitCombo->AddItem("MPH", NULL);
	m_pUnitCombo->AddItem("km/h", NULL);
	m_pUnitCombo->AddItem("None (Numbers)", NULL);

	m_pColorLabel = new Label(m_pHudPage, "ColorLabel", "Color:");
	m_pColorLabel->SetPos(10, 85);
	m_pColorLabel->SetSize(80, 24);

	m_pColorCombo = new ComboBox(m_pHudPage, "ColorCombo", 5, false);
	m_pColorCombo->SetPos(100, 85);
	m_pColorCombo->SetSize(140, 24);
	m_pColorCombo->AddItem("White", NULL);
	m_pColorCombo->AddItem("Green", NULL);
	m_pColorCombo->AddItem("Cyan", NULL);
	m_pColorCombo->AddItem("Yellow", NULL);
	m_pColorCombo->AddItem("Red", NULL);

	m_pSizeLabel = new Label(m_pHudPage, "SizeLabel", "Font Size:");
	m_pSizeLabel->SetPos(10, 115);
	m_pSizeLabel->SetSize(80, 24);

	m_pSizeEntry = new TextEntry(m_pHudPage, "SizeEntry");
	m_pSizeEntry->SetPos(100, 115);
	m_pSizeEntry->SetSize(60, 24);

	m_pXLabel = new Label(m_pHudPage, "XLabel", "Position X:");
	m_pXLabel->SetPos(10, 145);
	m_pXLabel->SetSize(80, 24);

	m_pXSlider = new Slider(m_pHudPage, "XSlider");
	m_pXSlider->SetPos(100, 145);
	m_pXSlider->SetSize(180, 24);
	m_pXSlider->SetRange(0, screenWide);
	m_pXSlider->AddActionSignalTarget(this);

	m_pXEntry = new TextEntry(m_pHudPage, "XEntry");
	m_pXEntry->SetPos(290, 145);
	m_pXEntry->SetSize(50, 24);

	m_pYLabel = new Label(m_pHudPage, "YLabel", "Position Y:");
	m_pYLabel->SetPos(10, 175);
	m_pYLabel->SetSize(80, 24);

	m_pYSlider = new Slider(m_pHudPage, "YSlider");
	m_pYSlider->SetPos(100, 175);
	m_pYSlider->SetSize(180, 24);
	m_pYSlider->SetRange(0, screenTall);
	m_pYSlider->AddActionSignalTarget(this);

	m_pYEntry = new TextEntry(m_pHudPage, "YEntry");
	m_pYEntry->SetPos(290, 175);
	m_pYEntry->SetSize(50, 24);

	// ==========================================
	// ----------- FPS COUNTER SECTION ----------
	// ==========================================
	m_pFPSHeader = new Label(m_pHudPage, "FPSHeader", "---- FPS Counter ----");
	m_pFPSHeader->SetPos(10, 215);
	m_pFPSHeader->SetSize(340, 20);

	m_pFPSCheck = new CheckButton(m_pHudPage, "FPSCheck", "Enable FPS Counter");
	m_pFPSCheck->SetPos(10, 238);
	m_pFPSCheck->SetSize(280, 24);

	m_pFPSColorLabel = new Label(m_pHudPage, "FPSColorLabel", "Color:");
	m_pFPSColorLabel->SetPos(10, 268);
	m_pFPSColorLabel->SetSize(80, 24);

	m_pFPSColorCombo = new ComboBox(m_pHudPage, "FPSColorCombo", 5, false);
	m_pFPSColorCombo->SetPos(100, 268);
	m_pFPSColorCombo->SetSize(140, 24);
	m_pFPSColorCombo->AddItem("White", NULL);
	m_pFPSColorCombo->AddItem("Green", NULL);
	m_pFPSColorCombo->AddItem("Cyan", NULL);
	m_pFPSColorCombo->AddItem("Yellow", NULL);
	m_pFPSColorCombo->AddItem("Red", NULL);

	m_pFPSSizeLabel = new Label(m_pHudPage, "FPSSizeLabel", "Font Size:");
	m_pFPSSizeLabel->SetPos(10, 298);
	m_pFPSSizeLabel->SetSize(80, 24);

	m_pFPSSizeEntry = new TextEntry(m_pHudPage, "FPSSizeEntry");
	m_pFPSSizeEntry->SetPos(100, 298);
	m_pFPSSizeEntry->SetSize(60, 24);

	m_pFPSXLabel = new Label(m_pHudPage, "FPSXLabel", "Position X:");
	m_pFPSXLabel->SetPos(10, 328);
	m_pFPSXLabel->SetSize(80, 24);

	m_pFPSXSlider = new Slider(m_pHudPage, "FPSXSlider");
	m_pFPSXSlider->SetPos(100, 328);
	m_pFPSXSlider->SetSize(180, 24);
	m_pFPSXSlider->SetRange(0, screenWide);
	m_pFPSXSlider->AddActionSignalTarget(this);

	m_pFPSXEntry = new TextEntry(m_pHudPage, "FPSXEntry");
	m_pFPSXEntry->SetPos(290, 328);
	m_pFPSXEntry->SetSize(50, 24);

	m_pFPSYLabel = new Label(m_pHudPage, "FPSYLabel", "Position Y:");
	m_pFPSYLabel->SetPos(10, 358);
	m_pFPSYLabel->SetSize(80, 24);

	m_pFPSYSlider = new Slider(m_pHudPage, "FPSYSlider");
	m_pFPSYSlider->SetPos(100, 358);
	m_pFPSYSlider->SetSize(180, 24);
	m_pFPSYSlider->SetRange(0, screenTall);
	m_pFPSYSlider->AddActionSignalTarget(this);

	m_pFPSYEntry = new TextEntry(m_pHudPage, "FPSYEntry");
	m_pFPSYEntry->SetPos(290, 358);
	m_pFPSYEntry->SetSize(50, 24);

	SetPos((screenWide - panelWide) / 2, (screenTall - panelTall) / 2);

	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SwitchTab(1);
}

CTFModMenu::~CTFModMenu()
{
}

void CTFModMenu::OnSliderMoved()
{
	char szVal[16];

	// Speedometer Sliders
	Q_snprintf(szVal, sizeof(szVal), "%d", m_pXSlider->GetValue());
	m_pXEntry->SetText(szVal);
	cl_speedometer_x.SetValue(m_pXSlider->GetValue());

	Q_snprintf(szVal, sizeof(szVal), "%d", m_pYSlider->GetValue());
	m_pYEntry->SetText(szVal);
	cl_speedometer_y.SetValue(m_pYSlider->GetValue());

	// FPS Sliders
	Q_snprintf(szVal, sizeof(szVal), "%d", m_pFPSXSlider->GetValue());
	m_pFPSXEntry->SetText(szVal);
	cl_fps_x.SetValue(m_pFPSXSlider->GetValue());

	Q_snprintf(szVal, sizeof(szVal), "%d", m_pFPSYSlider->GetValue());
	m_pFPSYEntry->SetText(szVal);
	cl_fps_y.SetValue(m_pFPSYSlider->GetValue());
}

void CTFModMenu::LoadSettings()
{
	int screenWide, screenTall;
	surface()->GetScreenSize(screenWide, screenTall);
	char szBuf[16];

	// --- Load Speedometer ---
	m_pSpeedometerCheck->SetSelected(cl_speedometer_enabled.GetBool());
	m_pUnitCombo->ActivateItemByRow(cl_speedometer_unit.GetInt());
	m_pColorCombo->ActivateItemByRow(cl_speedometer_color.GetInt());

	Q_snprintf(szBuf, sizeof(szBuf), "%d", cl_speedometer_size.GetInt());
	m_pSizeEntry->SetText(szBuf);

	int posX = cl_speedometer_x.GetInt();
	int posY = cl_speedometer_y.GetInt();

	if (posX < 0) posX = (screenWide - 200) / 2;
	if (posY < 0) posY = (screenTall / 2) + 30;

	m_pXSlider->SetValue(posX);
	Q_snprintf(szBuf, sizeof(szBuf), "%d", posX);
	m_pXEntry->SetText(szBuf);

	m_pYSlider->SetValue(posY);
	Q_snprintf(szBuf, sizeof(szBuf), "%d", posY);
	m_pYEntry->SetText(szBuf);

	// --- Load FPS ---
	m_pFPSCheck->SetSelected(cl_fps_enabled.GetBool());
	m_pFPSColorCombo->ActivateItemByRow(cl_fps_color.GetInt());

	Q_snprintf(szBuf, sizeof(szBuf), "%d", cl_fps_size.GetInt());
	m_pFPSSizeEntry->SetText(szBuf);

	int fpsX = cl_fps_x.GetInt();
	int fpsY = cl_fps_y.GetInt();

	m_pFPSXSlider->SetValue(fpsX);
	Q_snprintf(szBuf, sizeof(szBuf), "%d", fpsX);
	m_pFPSXEntry->SetText(szBuf);

	m_pFPSYSlider->SetValue(fpsY);
	Q_snprintf(szBuf, sizeof(szBuf), "%d", fpsY);
	m_pFPSYEntry->SetText(szBuf);
}

void CTFModMenu::SaveSettings()
{
	char szBuf[16];

	// --- Save Speedometer ---
	cl_speedometer_enabled.SetValue(m_pSpeedometerCheck->IsSelected() ? 1 : 0);
	cl_speedometer_unit.SetValue(m_pUnitCombo->GetActiveItem());
	cl_speedometer_color.SetValue(m_pColorCombo->GetActiveItem());

	m_pSizeEntry->GetText(szBuf, sizeof(szBuf));
	int newSize = atoi(szBuf);
	if (newSize > 0)
		cl_speedometer_size.SetValue(newSize);

	m_pXEntry->GetText(szBuf, sizeof(szBuf));
	cl_speedometer_x.SetValue(atoi(szBuf));

	m_pYEntry->GetText(szBuf, sizeof(szBuf));
	cl_speedometer_y.SetValue(atoi(szBuf));

	// --- Save FPS ---
	cl_fps_enabled.SetValue(m_pFPSCheck->IsSelected() ? 1 : 0);
	cl_fps_color.SetValue(m_pFPSColorCombo->GetActiveItem());

	m_pFPSSizeEntry->GetText(szBuf, sizeof(szBuf));
	int newFpsSize = atoi(szBuf);
	if (newFpsSize > 0)
		cl_fps_size.SetValue(newFpsSize);

	m_pFPSXEntry->GetText(szBuf, sizeof(szBuf));
	cl_fps_x.SetValue(atoi(szBuf));

	m_pFPSYEntry->GetText(szBuf, sizeof(szBuf));
	cl_fps_y.SetValue(atoi(szBuf));
}

void CTFModMenu::ApplySchemeSettings(IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(Color(20, 20, 20, 230));
}

void CTFModMenu::PerformLayout()
{
	BaseClass::PerformLayout();

	int wide, tall;
	GetSize(wide, tall);

	int pageWide = wide - 20;
	int pageTall = tall - 75;
	int tabWide = (pageWide / 3) - 5;

	m_pTabMovementBtn->SetSize(tabWide, 24);
	m_pTabHudBtn->SetPos(10 + tabWide + 5, 30);
	m_pTabHudBtn->SetSize(tabWide, 24);
	m_pTabStatsBtn->SetPos(10 + (tabWide + 5) * 2, 30);
	m_pTabStatsBtn->SetSize(tabWide, 24);

	m_pMovementPage->SetPos(10, 65);
	m_pMovementPage->SetSize(pageWide, pageTall);

	m_pHudPage->SetPos(10, 65);
	m_pHudPage->SetSize(pageWide, pageTall);

	m_pStatsPage->SetPos(10, 65);
	m_pStatsPage->SetSize(pageWide, pageTall);
}

void CTFModMenu::SwitchTab(int tabIndex)
{
	m_iActiveTab = tabIndex;
	m_pMovementPage->SetVisible(tabIndex == 0);
	m_pHudPage->SetVisible(tabIndex == 1);
	m_pStatsPage->SetVisible(tabIndex == 2);

	if (tabIndex == 2)
	{
		engine->ClientCmd("stats"); // demande de stats au serveur
	}
}

void CTFModMenu::OnCommand(const char* command)
{
	if (!Q_stricmp(command, "tab_movement"))
	{
		SwitchTab(0);
		return;
	}
	else if (!Q_stricmp(command, "tab_hud"))
	{
		SwitchTab(1);
		return;
	}
	else if (!Q_stricmp(command, "tab_stats"))
	{
		SwitchTab(2);
		return;
	}

	BaseClass::OnCommand(command);
}

void CTFModMenu::UpdateStatsDisplay()
{
	int iSeconds = GetClientStat_SecondsPlayed();
	int iHours = iSeconds / 3600;
	int iMinutes = (iSeconds % 3600) / 60;
	int iSecs = iSeconds % 60;

	char szBuf[64];
	Q_snprintf(szBuf, sizeof(szBuf), "Time Played: %d:%02d:%02d", iHours, iMinutes, iSecs);
	m_pStatsTimeLabel->SetText(szBuf);

	Q_snprintf(szBuf, sizeof(szBuf), "1v1 Wins: %d", GetClientStat_OneVOneWins());
	m_pStatsWinsLabel->SetText(szBuf);

	Q_snprintf(szBuf, sizeof(szBuf), "Total Kills: %d", GetClientStat_TotalKills());
	m_pStatsKillsLabel->SetText(szBuf);
}

void CTFModMenu::OnThink()
{
	BaseClass::OnThink();

	if (m_iActiveTab == 2)
	{
		UpdateStatsDisplay();
	}
}


CON_COMMAND(togglemodmenu, "Toggle the custom mod menu")
{
	ToggleModMenu();
}

void ToggleModMenu()
{
	if (!g_pModMenu)
	{
		g_pModMenu = new CTFModMenu(NULL);
		g_pModMenu->InvalidateLayout(true, true);
		g_pModMenu->SetVisible(false);
	}

	if (g_pModMenu->IsVisible())
	{
		g_pModMenu->SaveSettings();
		g_pModMenu->SetVisible(false);
	}
	else
	{
		g_pModMenu->LoadSettings();
		g_pModMenu->SetVisible(true);
		g_pModMenu->SetEnabled(true);
		g_pModMenu->MoveToFront();
		g_pModMenu->RequestFocus();
		g_pModMenu->Activate();
	}
}