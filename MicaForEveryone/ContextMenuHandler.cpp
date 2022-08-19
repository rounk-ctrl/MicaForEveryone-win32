#include "ContextMenuHandler.h"

LPCTSTR path = _T(".\\settings.ini");
void ShowContextMenu(HWND hwnd, POINT pt, HINSTANCE hInst, int DarkThemeEnabled)
{
	HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDC_MICAFOREVERYONE));
	if (hMenu)
	{
		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		if (hSubMenu)
		{
			SetForegroundWindow(hwnd);
			#pragma region Methods
			if (os.dwBuildNumber < 22000)
			{
				EnableMenuItem(hSubMenu, IDM_MICA, MF_GRAYED);
				EnableMenuItem(hSubMenu, IDM_ACRYLIC, MF_ENABLED);
				EnableMenuItem(hSubMenu, IDM_TABBED, MF_GRAYED);
			}
			if (os.dwBuildNumber == 22000)
			{
				EnableMenuItem(hSubMenu, IDM_MICA, MF_ENABLED);
				EnableMenuItem(hSubMenu, IDM_ACRYLIC, MF_ENABLED);
				EnableMenuItem(hSubMenu, IDM_TABBED, MF_GRAYED);
			}
			if (os.dwBuildNumber >= 22494)
			{
				EnableMenuItem(hSubMenu, IDM_MICA, MF_GRAYED);
				EnableMenuItem(hSubMenu, IDM_ACRYLIC, MF_GRAYED);
				EnableMenuItem(hSubMenu, IDM_TABBED, MF_GRAYED);
			}
			if (os.dwBuildNumber >= 22523)
			{
				EnableMenuItem(hSubMenu, IDM_MICA, MF_ENABLED);
				EnableMenuItem(hSubMenu, IDM_ACRYLIC, MF_ENABLED);
				EnableMenuItem(hSubMenu, IDM_TABBED, MF_ENABLED);
			}
			if (Extend)
			{
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MF_CHECKED;
				SetMenuItemInfo(hSubMenu, IDM_EXTEND, FALSE, &mi);

			}
			if (Dark)
			{
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MF_CHECKED;
				SetMenuItemInfo(hSubMenu, IDM_DARK, FALSE, &mi);
			}
			if (None)
			{
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MF_CHECKED;
				SetMenuItemInfo(hSubMenu, IDM_NONE, FALSE, &mi);
			}
			if (Light)
			{
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MF_CHECKED;
				SetMenuItemInfo(hSubMenu, IDM_LIGHT, FALSE, &mi);
			}
			if (Mica)
			{
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MF_CHECKED;
				SetMenuItemInfo(hSubMenu, IDM_MICA, FALSE, &mi);
			}
			if (DefaultBack)
			{
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MF_CHECKED;
				SetMenuItemInfo(hSubMenu, IDM_DEFAULTBACK, FALSE, &mi);
			}
			if (Acrylic)
			{
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MF_CHECKED;
				SetMenuItemInfo(hSubMenu, IDM_ACRYLIC, FALSE, &mi);
			}
			if (Tabbed)
			{
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MF_CHECKED;
				SetMenuItemInfo(hSubMenu, IDM_TABBED, FALSE, &mi);
			}
			if (DefaultCol)
			{
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MF_CHECKED;
				SetMenuItemInfo(hSubMenu, IDM_DEFCOL, FALSE, &mi);
			}
			if (SysCol)
			{
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MF_CHECKED;
				SetMenuItemInfo(hSubMenu, IDM_SYSCOL, FALSE, &mi);
			}
			if (DefCor)
			{
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MF_CHECKED;
				SetMenuItemInfo(hSubMenu, IDM_DEFCOR, FALSE, &mi);
			}
			if (Square)
			{
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MF_CHECKED;
				SetMenuItemInfo(hSubMenu, IDM_SQUARE, FALSE, &mi);
			}
			if (Round)
			{
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MF_CHECKED;
				SetMenuItemInfo(hSubMenu, IDM_ROUND, FALSE, &mi);
			}
			if (SRound)
			{
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MF_CHECKED;
				SetMenuItemInfo(hSubMenu, IDM_SROUND, FALSE, &mi);
			}
#pragma endregion
		}
		// respect menu drop alignment
		UINT uFlags = TPM_RIGHTBUTTON;
		if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
			uFlags |= TPM_RIGHTALIGN;
		else
			uFlags |= TPM_LEFTALIGN;

		// Use TPM_RETURNCMD flag let TrackPopupMenuEx function return the menu item identifier of the user's selection in the return value.
		uFlags |= TPM_RETURNCMD;
		if (hSubMenu != 0)
			menuItemId = TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hwnd, NULL);
		else
			MessageBox(hwnd, L"Error getting menu.", L"Error", MB_ICONSTOP);

		// Toggle the menu item state. 
		if (IDM_EXTEND == menuItemId)
		{
			if (Extend)
			{
				Extend = FALSE;
				WritePrivateProfileString(L"global", L"ExtendFrameIntoClientArea", L" False", path);
			}
			else
			{
				Extend = TRUE;
				WritePrivateProfileString(L"global", L"ExtendFrameIntoClientArea", L" True", path);
				MatchAndApplyRule(DarkThemeEnabled, 1, 0);
			}
		}
		if (IDM_EXIT == menuItemId)
		{
			Shell_NotifyIcon(NIM_DELETE, &nidApp);
			DestroyWindow(hwnd);
		}
		if (IDM_ABOUT == menuItemId)
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
		}
		if (IDM_DARK == menuItemId)
		{
			DefaultCol = FALSE;
			SysCol = FALSE;
			Dark = TRUE;
			Light = FALSE;
			WritePrivateProfileString(L"global", L"TitleBarColor", L" Dark", path);
			MatchAndApplyRule(DarkThemeEnabled, 1, 0);
		}
		if (IDM_LIGHT == menuItemId)
		{
			DefaultCol = FALSE;
			SysCol = FALSE;
			Light = TRUE;
			Dark = FALSE;
			WritePrivateProfileString(L"global", L"TitleBarColor", L" Light", path);
			MatchAndApplyRule(DarkThemeEnabled, 1, 0);
		}

		if (IDM_SYSCOL == menuItemId)
		{
			SysCol = TRUE;
			DefaultCol = FALSE;
			Light = FALSE;
			Dark = FALSE;
			WritePrivateProfileString(L"global", L"TitleBarColor", L" System", path);
			MatchAndApplyRule(DarkThemeEnabled, 1, 0);
		}
		if (IDM_DEFCOL == menuItemId)
		{
			SysCol = FALSE;
			DefaultCol = TRUE;
			Light = FALSE;
			Dark = FALSE;
			WritePrivateProfileString(L"global", L"TitleBarColor", L" Default", path);
		}
		if (IDM_NONE == menuItemId)
		{
			None = TRUE;
			Mica = FALSE;
			DefaultBack = FALSE;
			Acrylic = FALSE;
			Tabbed = FALSE;
			WritePrivateProfileString(L"global", L"BackdropPreference", L" None", path);
			MatchAndApplyRule(DarkThemeEnabled, 1, 0);
		}
		if (IDM_MICA == menuItemId)
		{
			None = FALSE;
			Mica = TRUE;
			DefaultBack = FALSE;
			Acrylic = FALSE;
			Tabbed = FALSE;
			WritePrivateProfileString(L"global", L"BackdropPreference", L" Mica", path);
			MatchAndApplyRule(DarkThemeEnabled, 1, 0);
		}
		if (IDM_ACRYLIC == menuItemId)
		{
			None = FALSE;
			DefaultBack = FALSE;
			Mica = FALSE;
			Acrylic = TRUE;
			Tabbed = FALSE;
			WritePrivateProfileString(L"global", L"BackdropPreference", L" Acrylic", path);
			MatchAndApplyRule(DarkThemeEnabled, 1, 0);
		}
		if (IDM_TABBED == menuItemId)
		{
			None = FALSE;
			Mica = FALSE;
			DefaultBack = FALSE;
			Acrylic = FALSE;
			Tabbed = TRUE;
			WritePrivateProfileString(L"global", L"BackdropPreference", L" Tabbed", path);
			MatchAndApplyRule(DarkThemeEnabled, 1, 0);
		}
		if (IDM_DEFAULTBACK == menuItemId)
		{
			None = FALSE;
			Mica = FALSE;
			DefaultBack = TRUE;
			Acrylic = FALSE;
			Tabbed = FALSE;
			WritePrivateProfileString(L"global", L"BackdropPreference", L" Default", path);
			MatchAndApplyRule(DarkThemeEnabled, 1, 0);
		}
		if (IDM_DEFCOR == menuItemId)
		{
			DefCor = TRUE;
			Square = FALSE;
			Round = FALSE;
			SRound = FALSE;
			WritePrivateProfileString(L"global", L"CornerPreference", L" Default", path);
			MatchAndApplyRule(DarkThemeEnabled, 1, 0);
		}
		if (IDM_SQUARE == menuItemId)
		{
			DefCor = FALSE;
			Square = TRUE;
			Round = FALSE;
			SRound = FALSE;
			WritePrivateProfileString(L"global", L"CornerPreference", L" Square", path);
			MatchAndApplyRule(DarkThemeEnabled, 1, 0);
		}
		if (IDM_ROUND == menuItemId)
		{
			DefCor = FALSE;
			Square = FALSE;
			Round = TRUE;
			SRound = FALSE;
			WritePrivateProfileString(L"global", L"CornerPreference", L" Rounded", path);
			MatchAndApplyRule(DarkThemeEnabled, 1, 0);
		}
		if (IDM_SROUND == menuItemId)
		{
			DefCor = FALSE;
			Square = FALSE;
			Round = FALSE;
			SRound = TRUE;
			WritePrivateProfileString(L"global", L"CornerPreference", L" RoundedSmall", path);
			MatchAndApplyRule(DarkThemeEnabled, 1, 0);
		}
		DestroyMenu(hMenu);
	}
}
