#pragma once

#include "resource.h"
#include "framework.h"

#define DWMWA_USE_IMMERSIVE_DARK_MODE 20

enum IMMERSIVE_HC_CACHE_MODE
{
	IHCM_USE_CACHED_VALUE,
	IHCM_REFRESH
};
enum class PreferredAppMode
{
	Default,
	AllowDark,
	ForceDark,
	ForceLight,
	Max
};

// 1809 17763
using fnShouldAppsUseDarkMode = bool (WINAPI*)(); // ordinal 132
using fnAllowDarkModeForWindow = bool (WINAPI*)(HWND hWnd, bool allow); // ordinal 133
using fnFlushMenuThemes = void (WINAPI*)(); // ordinal 136
using fnRefreshImmersiveColorPolicyState = void (WINAPI*)(); // ordinal 104
using fnIsDarkModeAllowedForWindow = bool (WINAPI*)(HWND hWnd); // ordinal 137
using fnGetIsImmersiveColorUsingHighContrast = bool (WINAPI*)(IMMERSIVE_HC_CACHE_MODE mode); // ordinal 106
using fnOpenNcThemeData = HTHEME(WINAPI*)(HWND hWnd, LPCWSTR pszClassList); // ordinal 49
// 1903 18362
using fnShouldSystemUseDarkMode = bool (WINAPI*)(); // ordinal 138
using fnSetPreferredAppMode = PreferredAppMode(WINAPI*)(PreferredAppMode appMode); // ordinal 135, in 1903
using fnIsDarkModeAllowedForApp = bool (WINAPI*)(); // ordinal 

/// Variables
// Notification Icon
NOTIFYICONDATA nidApp;

// Menu items values
BOOL Extend = FALSE;
BOOL Dark = FALSE;
BOOL Light = FALSE;
BOOL Mica = FALSE;
BOOL DefaultBack = TRUE;
BOOL Acrylic = FALSE;
BOOL Tabbed = FALSE;
BOOL DefaultCol = TRUE;

// value for DwmSetWindowAttribute
DWORD nice = TRUE;

// WinEventHook
HWINEVENTHOOK hEvent;

// list for all top level windows
std::vector<HWND> hwndlist;

// DwmExtendFrameIntoClientArea margins
MARGINS margins = { -1 };

// Menu item info
MENUITEMINFO mi = { 0 };

// osversionex struct
OSVERSIONINFOEX os;

// Menu Item id int used to check return values from menu
UINT menuItemId = 0;

// Dark mode text and background color
constexpr COLORREF darkBkColor = 0x202020;
constexpr COLORREF darkTextColor = 0xFFFFFF;

// icon
HICON hMainIcon;

// point used to spawn the menu where your cursor is
POINT ok;

// some functions used for dark mode
fnSetPreferredAppMode SetPreferredAppMode;
fnRefreshImmersiveColorPolicyState RefreshImmersiveColorPolicyState;
fnAllowDarkModeForWindow AllowDarkModeForWindow;

HWND Title;
HFONT hFont;

static HBRUSH hbrBkgnd = nullptr;
