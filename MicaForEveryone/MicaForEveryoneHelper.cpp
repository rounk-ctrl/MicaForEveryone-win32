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
