#include "cbase.h"
#include "tf_hud_playmenu.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include "iclientmode.h"
#include <vgui/IScheme.h>
#include <vgui_controls/Controls.h>
#include "cdll_int.h" // Needed for engine access

using namespace vgui;

static CTFPlayMenu* g_pPlayMenu = NULL;

CTFPlayMenu::CTFPlayMenu(vgui::Panel* parent) : BaseClass(parent, "TFPlayMenu")
{
	HScheme scheme = vgui::scheme()->LoadSchemeFromFile("resource/ClientScheme.res", "ClientScheme");
	SetScheme(scheme);

	SetTitle("PLAY MENU", true);
	SetMoveable(true);
	SetSizeable(false);
	SetVisible(false);
	SetProportional(false);

	SetMinimumSize(420, 480);
	SetSize(420, 480);

	// ==========================================
	// --- MAIN NAVIGATION TABS ---
	// ==========================================
	m_pTabSingleplayerBtn = new Button(this, "TabSingleplayerBtn", "Singleplayer", this, "tab_singleplayer");
	m_pTabMultiplayerBtn = new Button(this, "TabMultiplayerBtn", "Multiplayer", this, "tab_multiplayer");

	// ==========================================
	// --- SUB NAVIGATION TABS ---
	// ==========================================
	m_pSubTabCoopBtn = new Button(this, "SubTabCoopBtn", "Co-op Mode", this, "subtab_coop");
	m_pSubTab1v1Btn = new Button(this, "SubTab1v1Btn", "1v1 Arena", this, "subtab_1v1");

	// ==========================================
	// --- PAGES ---
	// ==========================================
	m_pSingleplayerPage = new Panel(this, "SingleplayerPage");
	m_pCoopPage = new Panel(this, "CoopPage");
	m_p1v1Page = new Panel(this, "1v1Page");

	// ---------------- Singleplayer Page ----------------
	m_pSPMapLabel = new Label(m_pSingleplayerPage, "SPMapLabel", "Select Map:");
	m_pSPMapCombo = new ComboBox(m_pSingleplayerPage, "SPMapCombo", 6, false);
	m_pSPMapCombo->AddItem("Sandbox", NULL);
	m_pSPMapCombo->AddItem("Map 1 (placeholder)", NULL);
	m_pSPMapCombo->AddItem("Map 2 (placeholder)", NULL);
	m_pSPMapCombo->ActivateItem(0);

	m_pSPDiffLabel = new Label(m_pSingleplayerPage, "SPDiffLabel", "Select Difficulty:");
	m_pSPDifficultyCombo = new ComboBox(m_pSingleplayerPage, "SPDifficultyCombo", 6, false);
	m_pSPDifficultyCombo->AddItem("Easy", NULL);
	m_pSPDifficultyCombo->AddItem("Normal", NULL);
	m_pSPDifficultyCombo->AddItem("Hard", NULL);
	m_pSPDifficultyCombo->ActivateItem(0);

	m_pSPPlayButton = new Button(m_pSingleplayerPage, "SPPlayButton", "LAUNCH SINGLEPLAYER", this, "start_game");

	// ---------------- Co-op Page ----------------
	m_pCoopMapLabel = new Label(m_pCoopPage, "CoopMapLabel", "Select Map:");
	m_pCoopMapCombo = new ComboBox(m_pCoopPage, "CoopMapCombo", 4, false);
	m_pCoopMapCombo->AddItem("Coop_Sandbox", NULL);
	m_pCoopMapCombo->AddItem("Coop_Map1", NULL);
	m_pCoopMapCombo->ActivateItem(0);

	m_pCoopDiffLabel = new Label(m_pCoopPage, "CoopDiffLabel", "Select Difficulty:");
	m_pCoopDiffCombo = new ComboBox(m_pCoopPage, "CoopDiffCombo", 4, false);
	m_pCoopDiffCombo->AddItem("Normal", NULL);
	m_pCoopDiffCombo->AddItem("Hard", NULL);
	m_pCoopDiffCombo->ActivateItem(0);

	m_pCoopIPLabel = new Label(m_pCoopPage, "CoopIPLabel", "Your Host Address:");
	m_pCoopIPEntry = new TextEntry(m_pCoopPage, "CoopIPEntry");
	m_pCoopIPEntry->SetText("127.0.0.1:27015");

	m_pCoopHostBtn = new Button(m_pCoopPage, "CoopHostBtn", "HOST CO-OP SERVER", this, "start_coop");

	m_pCoopJoinLabel = new Label(m_pCoopPage, "CoopJoinLabel", "Direct Connect IP:");
	m_pCoopJoinIPEntry = new TextEntry(m_pCoopPage, "CoopJoinIPEntry");
	m_pCoopJoinIPEntry->SetText("");

	m_pCoopJoinBtn = new Button(m_pCoopPage, "CoopJoinBtn", "JOIN CO-OP SERVER", this, "join_coop");

	// ---------------- 1v1 Page ----------------
	m_p1v1MapLabel = new Label(m_p1v1Page, "1v1MapLabel", "Select Arena Map:");
	m_p1v1MapCombo = new ComboBox(m_p1v1Page, "1v1MapCombo", 4, false);
	m_p1v1MapCombo->AddItem("pvp_flatarena", NULL);
	m_p1v1MapCombo->AddItem("idk", NULL);
	m_p1v1MapCombo->ActivateItem(0);

	m_p1v1RoundsLabel = new Label(m_p1v1Page, "1v1RoundsLabel", "Rounds to Win:");
	m_p1v1RoundsEntry = new TextEntry(m_p1v1Page, "1v1RoundsEntry");
	m_p1v1RoundsEntry->SetText("20");

	m_p1v1NoHookCheck = new CheckButton(m_p1v1Page, "1v1NoHook", "Disable Grappling Hook");
	m_p1v1NoSapperCheck = new CheckButton(m_p1v1Page, "1v1NoSapper", "Disable Sapper");
	m_p1v1NoKnifeCheck = new CheckButton(m_p1v1Page, "1v1NoKnife", "Disable Knife");

	m_p1v1IPLabel = new Label(m_p1v1Page, "1v1IPLabel", "Your Host Address:");
	m_p1v1IPEntry = new TextEntry(m_p1v1Page, "1v1IPEntry");
	m_p1v1IPEntry->SetText("127.0.0.1:27015");

	m_p1v1HostBtn = new Button(m_p1v1Page, "1v1HostBtn", "HOST 1v1 MATCH", this, "start_1v1");

	m_p1v1JoinLabel = new Label(m_p1v1Page, "1v1JoinLabel", "Opponent IP:");
	m_p1v1JoinIPEntry = new TextEntry(m_p1v1Page, "1v1JoinIPEntry");
	m_p1v1JoinIPEntry->SetText("");

	m_p1v1JoinBtn = new Button(m_p1v1Page, "1v1JoinBtn", "JOIN 1v1 MATCH", this, "join_1v1");

	int screenWide, screenTall;
	surface()->GetScreenSize(screenWide, screenTall);
	SetPos((screenWide - 420) / 2, (screenTall - 480) / 2);

	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SwitchTab(0, 0);
}

CTFPlayMenu::~CTFPlayMenu()
{
}

void CTFPlayMenu::ApplySchemeSettings(IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	// TF2 Option Window Theme Colors
	SetBgColor(Color(38, 35, 33, 245));         // Dark TF Charcoal background
	SetBorder(pScheme->GetBorder("BaseBorder"));

	// Apply TF2 styling to pages
	Color pageBg = Color(46, 43, 40, 200);
	m_pSingleplayerPage->SetBgColor(pageBg);
	m_pCoopPage->SetBgColor(pageBg);
	m_p1v1Page->SetBgColor(pageBg);

	m_pSingleplayerPage->SetBorder(pScheme->GetBorder("ButtonDepressedBorder"));
	m_pCoopPage->SetBorder(pScheme->GetBorder("ButtonDepressedBorder"));
	m_p1v1Page->SetBorder(pScheme->GetBorder("ButtonDepressedBorder"));
}

void CTFPlayMenu::PerformLayout()
{
	BaseClass::PerformLayout();

	int wide, tall;
	GetSize(wide, tall);

	int margin = 16;
	int contentWide = wide - (margin * 2);

	// --- MAIN TABS ---
	int mainTabY = 32;
	int mainBtnWide = (contentWide - 6) / 2;

	m_pTabSingleplayerBtn->SetPos(margin, mainTabY);
	m_pTabSingleplayerBtn->SetSize(mainBtnWide, 26);

	m_pTabMultiplayerBtn->SetPos(margin + mainBtnWide + 6, mainTabY);
	m_pTabMultiplayerBtn->SetSize(mainBtnWide, 26);

	bool isMultiplayer = (m_iActiveMainTab == 1);
	m_pSubTabCoopBtn->SetVisible(isMultiplayer);
	m_pSubTab1v1Btn->SetVisible(isMultiplayer);

	int pageTop = 68;
	if (isMultiplayer)
	{
		int subBtnWide = (contentWide - 6) / 2;
		int subTabY = 64;

		m_pSubTabCoopBtn->SetPos(margin, subTabY);
		m_pSubTabCoopBtn->SetSize(subBtnWide, 22);

		m_pSubTab1v1Btn->SetPos(margin + subBtnWide + 6, subTabY);
		m_pSubTab1v1Btn->SetSize(subBtnWide, 22);

		pageTop = 92;
	}

	int pageTall = tall - pageTop - margin;

	m_pSingleplayerPage->SetBounds(margin, pageTop, contentWide, pageTall);
	m_pCoopPage->SetBounds(margin, pageTop, contentWide, pageTall);
	m_p1v1Page->SetBounds(margin, pageTop, contentWide, pageTall);

	int pMargin = 12;
	int pWide = contentWide - (pMargin * 2);

	//SINGLEPLAYER LAYOUT
	m_pSPMapLabel->SetBounds(pMargin, 12, pWide, 18);
	m_pSPMapCombo->SetBounds(pMargin, 30, pWide, 24);

	m_pSPDiffLabel->SetBounds(pMargin, 66, pWide, 18);
	m_pSPDifficultyCombo->SetBounds(pMargin, 84, pWide, 24);

	m_pSPPlayButton->SetBounds(pMargin, pageTall - 48, pWide, 36);

	//CO-OP LAYOUT
	m_pCoopMapLabel->SetBounds(pMargin, 8, pWide, 16);
	m_pCoopMapCombo->SetBounds(pMargin, 24, pWide, 22);

	m_pCoopDiffLabel->SetBounds(pMargin, 50, pWide, 16);
	m_pCoopDiffCombo->SetBounds(pMargin, 66, pWide, 22);

	m_pCoopIPLabel->SetBounds(pMargin, 94, pWide, 16);
	m_pCoopIPEntry->SetBounds(pMargin, 110, pWide, 22);
	m_pCoopHostBtn->SetBounds(pMargin, 136, pWide, 28);

	m_pCoopJoinLabel->SetBounds(pMargin, 180, pWide, 16);
	m_pCoopJoinIPEntry->SetBounds(pMargin, 196, pWide, 22);
	m_pCoopJoinBtn->SetBounds(pMargin, 222, pWide, 28);

	// --- 1V1 LAYOUT ---
	m_p1v1MapLabel->SetBounds(pMargin, 8, pWide, 16);
	m_p1v1MapCombo->SetBounds(pMargin, 24, pWide, 22);

	m_p1v1RoundsLabel->SetBounds(pMargin, 52, 110, 20);
	m_p1v1RoundsEntry->SetBounds(120, 52, 60, 20);

	m_p1v1NoHookCheck->SetBounds(pMargin, 76, pWide, 18);
	m_p1v1NoSapperCheck->SetBounds(pMargin, 94, pWide, 18);
	m_p1v1NoKnifeCheck->SetBounds(pMargin, 112, pWide, 18);

	m_p1v1IPLabel->SetBounds(pMargin, 136, pWide, 16);
	m_p1v1IPEntry->SetBounds(pMargin, 152, pWide, 22);
	m_p1v1HostBtn->SetBounds(pMargin, 178, pWide, 28);

	m_p1v1JoinLabel->SetBounds(pMargin, 218, pWide, 16);
	m_p1v1JoinIPEntry->SetBounds(pMargin, 234, pWide, 22);
	m_p1v1JoinBtn->SetBounds(pMargin, 260, pWide, 28);
}

void CTFPlayMenu::SwitchTab(int mainTab, int subTab)
{
	m_iActiveMainTab = mainTab;
	m_iActiveSubTab = subTab;

	m_pSingleplayerPage->SetVisible(mainTab == 0);
	m_pCoopPage->SetVisible(mainTab == 1 && subTab == 0);
	m_p1v1Page->SetVisible(mainTab == 1 && subTab == 1);

	// Highlight active main tab button
	m_pTabSingleplayerBtn->SetSelected(mainTab == 0);
	m_pTabMultiplayerBtn->SetSelected(mainTab == 1);

	// Highlight active sub tab button
	m_pSubTabCoopBtn->SetSelected(subTab == 0);
	m_pSubTab1v1Btn->SetSelected(subTab == 1);

	InvalidateLayout(true, false);
}

void CTFPlayMenu::OnCommand(const char* command)
{
	if (!Q_stricmp(command, "tab_singleplayer"))
	{
		SwitchTab(0);
		return;
	}
	else if (!Q_stricmp(command, "tab_multiplayer"))
	{
		SwitchTab(1, m_iActiveSubTab);
		return;
	}
	else if (!Q_stricmp(command, "subtab_coop"))
	{
		SwitchTab(1, 0);
		return;
	}
	else if (!Q_stricmp(command, "subtab_1v1"))
	{
		SwitchTab(1, 1);
		return;
	}
	else if (!Q_stricmp(command, "start_game"))
	{
		Msg("Starting Singleplayer. Selected map index: %d, difficulty index: %d\n",
			m_pSPMapCombo->GetActiveItem(), m_pSPDifficultyCombo->GetActiveItem());
		return;
	}
	else if (!Q_stricmp(command, "start_coop"))
	{
		Msg("Hosting Co-op on Map Index: %d, Diff: %d\n", m_pCoopMapCombo->GetActiveItem(), m_pCoopDiffCombo->GetActiveItem());
		return;
	}
	else if (!Q_stricmp(command, "join_coop"))
	{
		char szIP[64];
		m_pCoopJoinIPEntry->GetText(szIP, sizeof(szIP));
		Msg("Connecting to Co-op host: %s\n", szIP);
		return;
	}
	else if (!Q_stricmp(command, "start_1v1"))
	{
		char szMapName[64] = "pvp_flatarena";
		if (m_p1v1MapCombo)
		{
			m_p1v1MapCombo->GetText(szMapName, sizeof(szMapName));
		}

		int iRounds = 20;
		if (m_p1v1RoundsEntry)
		{
			char szRounds[16];
			m_p1v1RoundsEntry->GetText(szRounds, sizeof(szRounds));
			if (Q_strlen(szRounds) > 0)
			{
				iRounds = atoi(szRounds);
			}
		}

		char szCmd[256];
		Q_snprintf(szCmd, sizeof(szCmd), "map %s +sv_force_spy_mode 1 +start_1v1_mode %d\n", szMapName, iRounds);

		engine->ClientCmd_Unrestricted(szCmd);

		SetVisible(false);
		return;
	}
	else if (!Q_stricmp(command, "join_1v1"))
	{
		char szIP[64];
		m_p1v1JoinIPEntry->GetText(szIP, sizeof(szIP));
		if (Q_strlen(szIP) > 0)
		{
			char szConnectCmd[128];
			Q_snprintf(szConnectCmd, sizeof(szConnectCmd), "connect %s\n", szIP);
			engine->ClientCmd_Unrestricted(szConnectCmd);
			SetVisible(false);
		}
		return;
	}

	BaseClass::OnCommand(command);
}

CON_COMMAND(toggleplaymenu, "Toggle the custom play menu")
{
	ToggleplayMenu();
}

void ToggleplayMenu()
{
	if (!g_pPlayMenu)
	{
		g_pPlayMenu = new CTFPlayMenu(NULL);
		g_pPlayMenu->InvalidateLayout(true, true);
		g_pPlayMenu->SetVisible(false);
	}

	if (g_pPlayMenu->IsVisible())
	{
		g_pPlayMenu->SetVisible(false);
	}
	else
	{
		g_pPlayMenu->SetVisible(true);
		g_pPlayMenu->SetEnabled(true);
		g_pPlayMenu->MoveToFront();
		g_pPlayMenu->RequestFocus();
		g_pPlayMenu->Activate();
	}
}