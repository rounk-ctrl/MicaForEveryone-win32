#include "MicaForEveryoneHelper.h"
/// Variables
// Notification Icon
NOTIFYICONDATA nidApp;

// Menu items values
BOOL Extend;
BOOL Dark;
BOOL Light;
BOOL Mica;
BOOL DefaultBack;
BOOL None;
BOOL Acrylic;
BOOL Tabbed;
BOOL DefaultCol;
BOOL SysCol;

BOOL DefCor;
BOOL Square;
BOOL Round;
BOOL SRound;

std::vector<std::string> rulelist;
std::vector<std::string> processlist;

// WinEventHook
HWINEVENTHOOK hEvent;

// list for all top level windows
std::vector<Window> hwndlist;
std::vector<TCHAR*> procnamelist;

// DwmExtendFrameIntoClientArea margins
MARGINS margins = { -1 };

// Menu item info
MENUITEMINFO mi = { 0 };

// osversionex struct
OSVERSIONINFOEX os;

// Menu Item id int used to check return values from menu
UINT menuItemId = 0;


// icon
HICON hMainIcon;

// point used to spawn the menu where your cursor is
POINT ok;
HKEY hKeyPersonalization;

BOOL ApplyDarkTitleBar(HWND hwnd, BOOL mode)
{
	DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &mode, sizeof mode);
	return TRUE;
}

BOOL CobaltMicaEffect(HWND hwnd, BOOL allow)
{
	DwmSetWindowAttribute(hwnd, DWMWA_MICA_EFFECT, &allow, sizeof allow);
	return TRUE;
}

void SetSystemBackdropType(HWND hwnd, SystemBackdropType value)
{
	DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &value, sizeof value);
}

void SetWindowRoundPreference(HWND hwnd, DWM_WINDOW_CORNER_PREFERENCE value)
{
	DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &value, sizeof value);
}

BOOL ApplyMica(HWND hwnd)
{
	if (os.dwBuildNumber == 22000)
	{
		CobaltMicaEffect(hwnd, TRUE);
	}
	else if (os.dwBuildNumber >= 22523)
	{
		SetSystemBackdropType(hwnd, SystemBackdropType::Mica);
	}
	return TRUE;
}

BOOL ApplyAcrylic(HWND hwnd)
{
	if (os.dwBuildNumber <= 22000)
	{
		SetWindowBlur(hwnd);
	}
	else if (os.dwBuildNumber >= 22523)
	{
		SetSystemBackdropType(hwnd, SystemBackdropType::Acrylic);
	}
	return TRUE;
}

BOOL ApplyTabbed(HWND hwnd)
{
	SetSystemBackdropType(hwnd, SystemBackdropType::Tabbed);
	return TRUE;
}

BOOL ApplyDefaultMaterial(HWND hwnd)
{
	SetSystemBackdropType(hwnd, SystemBackdropType::Auto);
	return TRUE;
}
BOOL ApplyNoMaterial(HWND hwnd)
{
	SetSystemBackdropType(hwnd, SystemBackdropType::None);
	return TRUE;
}

void SetWindowBlur(HWND hWnd)
{
	const HINSTANCE hModule = LoadLibrary(TEXT("user32.dll"));
	if (hModule)
	{
		struct ACCENTPOLICY
		{
			int nAccentState;
			int nFlags;
			int nColor;
			int nAnimationId;
		};
		struct WINCOMPATTRDATA
		{
			int nAttribute;
			PVOID pData;
			ULONG ulDataSize;
		};
		typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
		const pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hModule, "SetWindowCompositionAttribute");
		if (SetWindowCompositionAttribute)
		{
			ACCENTPOLICY policy = { 3, 0, 0, 0 };
			WINCOMPATTRDATA data = { 19, &policy, sizeof(ACCENTPOLICY) }; // WCA_ACCENT_POLICY=19
			SetWindowCompositionAttribute(hWnd, &data);
		}
		FreeLibrary(hModule);
	}
}

BOOL TrayIcon(HWND hWnd, HINSTANCE hInst)
{
	hMainIcon = LoadIcon(hInst, (LPCTSTR)MAKEINTRESOURCE(IDI_MICAFOREVERYONE));
	nidApp.cbSize = sizeof(NOTIFYICONDATA);
	nidApp.hWnd = (HWND)hWnd;
	nidApp.uID = IDI_MICAFOREVERYONE;
	nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nidApp.hIcon = hMainIcon;
	nidApp.uCallbackMessage = WM_USER_SHELLICON;
	nidApp.uVersion = NOTIFYICON_VERSION_4;
	LoadString(hInst, IDS_APPTOOLTIP, nidApp.szTip, MAX_LOADSTRING);
	Shell_NotifyIcon(NIM_ADD, &nidApp);
	return TRUE;
}

void DisableMaximizeButton(HWND hwnd)
{
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
}

BOOL RtlGetVersion(OSVERSIONINFOEX* os) {
	HMODULE hMod;
	RtlGetVersion_FUNC func;
#ifdef UNICODE
	OSVERSIONINFOEXW* osw = os;
#else
	OSVERSIONINFOEXW o;
	OSVERSIONINFOEXW* osw = &o;
#endif

	hMod = LoadLibrary(TEXT("ntdll.dll"));
	if (hMod) {
		func = (RtlGetVersion_FUNC)GetProcAddress(hMod, "RtlGetVersion");
		if (func == 0) {
			FreeLibrary(hMod);
			return FALSE;
		}
		ZeroMemory(osw, sizeof(*osw));
		osw->dwOSVersionInfoSize = sizeof(*osw);
		func(osw);
#ifndef UNICODE
		os->dwBuildNumber = osw->dwBuildNumber;
		os->dwMajorVersion = osw->dwMajorVersion;
		os->dwMinorVersion = osw->dwMinorVersion;
		os->dwPlatformId = osw->dwPlatformId;
		os->dwOSVersionInfoSize = sizeof(*os);
		DWORD sz = sizeof(os->szCSDVersion);
		WCHAR* src = osw->szCSDVersion;
		unsigned char* dtc = (unsigned char*)os->szCSDVersion;
		while (*src)
			*Dtc++ = (unsigned char)*src++;
		*Dtc = '\ 0';
#endif

	}
	else
		return FALSE;
	FreeLibrary(hMod);
	return TRUE;
}
int IsExplorerDarkTheme()
{
	RegOpenKeyEx(
		HKEY_CURRENT_USER,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
		0, KEY_READ, &hKeyPersonalization
	);
	DWORD dwBufferSize(sizeof(DWORD));
	DWORD nResult(0);
	LONG nError = RegQueryValueEx(
		hKeyPersonalization,
		L"AppsUseLightTheme",
		0,
		NULL,
		reinterpret_cast<LPBYTE>(&nResult),
		&dwBufferSize
	);
	return ERROR_SUCCESS == nError ? !nResult : FALSE;
}
const LPCTSTR path = _T(".\\settings.ini");
void UpdateConfig(std::wstring name)
{
	TCHAR value[64];
	GetPrivateProfileString(name.c_str(), _T("TitleBarColor"), _T("Default"), value, 64, path);
	if (!_tcscmp(value, _T("Default")))
		DefaultCol = TRUE;
	else if (!_tcscmp(value, _T("System")))
		SysCol = TRUE;
	else if (!_tcscmp(value, _T("Light")))
		Light = TRUE;
	else if (!_tcscmp(value, _T("Dark")))
		Dark = TRUE;
	else
	{
		SysCol = FALSE;
		Dark = FALSE;
		Light = FALSE;
		DefaultCol = TRUE;
	}

	GetPrivateProfileString(name.c_str(), _T("BackdropPreference"), _T("Default"), value, 64, path);
	if (!_tcscmp(value, _T("Default")))
		DefaultBack = TRUE;
	else if (!_tcscmp(value, _T("None")))
		None = TRUE;
	else if (!_tcscmp(value, _T("Mica")))
		Mica = TRUE;
	else if (!_tcscmp(value, _T("Acrylic")))
		Acrylic = TRUE;
	else if (!_tcscmp(value, _T("Tabbed")))
		Tabbed = TRUE;
	else
	{
		None = FALSE;
		Mica = FALSE;
		Acrylic = FALSE;
		Tabbed = FALSE;
		DefaultBack = TRUE;
	}

	GetPrivateProfileString(name.c_str(), _T("ExtendFrameIntoClientArea"), _T("False"), value, 64, path);
	if (!_tcscmp(value, _T("False")))
		Extend = FALSE;
	else if (!_tcscmp(value, _T("True")))
		Extend = TRUE;
	else
		Extend = FALSE;

	GetPrivateProfileString(name.c_str(), _T("CornerPreference"), _T("Default"), value, 64, path);
	if (!_tcscmp(value, _T("Default")))
		DefCor = TRUE;
	else if (!_tcscmp(value, _T("Square")))
		Square = TRUE;
	else if (!_tcscmp(value, _T("Rounded")))
		Round = TRUE;
	else if (!_tcscmp(value, _T("RoundedSmall")))
		SRound = TRUE;
	else
	{
		Square = FALSE;
		Round = FALSE;
		SRound = FALSE;
		DefCor = TRUE;
	}
}
void EnableDebugPriv()
{
	HANDLE hToken;
	LUID luid;
	TOKEN_PRIVILEGES tkp;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL);

	CloseHandle(hToken);
}
void ApplyRule(int DarkThemeEnabled, int type, HWND hwnd, Window window)
{
	if (Extend)
		DwmExtendFrameIntoClientArea(window.GetHwnd(), &margins);
	else if (SysCol)
		ApplyDarkTitleBar(window.GetHwnd(), DarkThemeEnabled);
	else if (Light)
		ApplyDarkTitleBar(window.GetHwnd(), FALSE);
	else if (Dark)
		ApplyDarkTitleBar(window.GetHwnd(), TRUE);
	else if (None)
		ApplyNoMaterial(window.GetHwnd());
	else if (Mica)
		ApplyMica(window.GetHwnd());
	else if (Acrylic)
		ApplyAcrylic(window.GetHwnd());
	else if (Tabbed)
		ApplyTabbed(window.GetHwnd());
	else if (DefaultBack)
		ApplyDefaultMaterial(window.GetHwnd());
	else if (DefCor)
		SetWindowRoundPreference(window.GetHwnd(), DWMWCP_DEFAULT);
	else if (Square)
		SetWindowRoundPreference(window.GetHwnd(), DWMWCP_DONOTROUND);
	else if (Round)
		SetWindowRoundPreference(window.GetHwnd(), DWMWCP_ROUND);
	else if (SRound)
		SetWindowRoundPreference(window.GetHwnd(), DWMWCP_ROUNDSMALL);
}
void MatchAndApplyRule(int DarkThemeEnabled, int type, HWND hwnd)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	BOOL match = FALSE;
	if (type == 1)
	{
		for (const Window& window : hwndlist)
		{
			for (std::string string : rulelist)
			{
				std::wstring wide = converter.from_bytes(string);
				if (wide.find(window.GetProcName()) != std::string::npos)
				{
					match = TRUE;
					UpdateConfig(wide);
					ApplyRule(DarkThemeEnabled, type, hwnd, window);
					Window newwin(window.GetHwnd(), window.GetPid(), window.GetProcName());
					// figure out how to remove this from the list
				}
				else if (wide.find(window.GetProcName()) == std::string::npos)
				{
					match = FALSE;
					UpdateConfig(L"global");
					MessageBox(0, window.GetProcName().c_str(), 0, 0);
					ApplyRule(DarkThemeEnabled, type, hwnd, window);
				}
			}
		}
	}
	else if (type == 2)
	{
		if (Extend)
			DwmExtendFrameIntoClientArea(hwnd, &margins);
		else if (SysCol)
			ApplyDarkTitleBar(hwnd, DarkThemeEnabled);
		else if (Light)
			ApplyDarkTitleBar(hwnd, FALSE);
		else if (Dark)
			ApplyDarkTitleBar(hwnd, TRUE);
		else if (None)
			ApplyNoMaterial(hwnd);
		else if (Mica)
			ApplyMica(hwnd);
		else if (Acrylic)
			ApplyAcrylic(hwnd);
		else if (Tabbed)
			ApplyTabbed(hwnd);
		else if (DefaultBack)
			ApplyDefaultMaterial(hwnd);
		else if (DefCor)
			SetWindowRoundPreference(hwnd, DWMWCP_DEFAULT);
		else if (Square)
			SetWindowRoundPreference(hwnd, DWMWCP_DONOTROUND);
		else if (Round)
			SetWindowRoundPreference(hwnd, DWMWCP_ROUND);
		else if (SRound)
			SetWindowRoundPreference(hwnd, DWMWCP_ROUNDSMALL);
	}
}
