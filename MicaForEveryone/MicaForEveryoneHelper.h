#pragma once
#include "framework.h"
#include "resource.h"

typedef void (WINAPI* RtlGetVersion_FUNC) (OSVERSIONINFOEXW*);

#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#define DWMWA_MICA_EFFECT 1029
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#define MAX_LOADSTRING 100
#define	WM_USER_SHELLICON WM_USER + 1

enum class SystemBackdropType 
{
	Auto,
	None,
	Mica,
	Acrylic,
	Tabbed
};
extern BOOL Extend;
extern BOOL Dark;
extern BOOL Light;
extern BOOL Mica;
extern BOOL DefaultBack;
extern BOOL Acrylic;
extern BOOL Tabbed;
extern BOOL DefaultCol;
extern NOTIFYICONDATA nidApp;
extern HWINEVENTHOOK hEvent;
extern std::vector<HWND> hwndlist;
extern MARGINS margins;
extern MENUITEMINFO mi;
extern OSVERSIONINFOEX os;
extern UINT menuItemId;
extern HICON hMainIcon;
extern POINT ok;


BOOL ApplyDarkTitleBar(HWND hwnd, BOOL mode);
BOOL CobaltMicaEffect(HWND hwnd, BOOL allow);
void SetSystemBackdropType(HWND hwnd, SystemBackdropType value);
BOOL ApplyMica(HWND hwnd);
BOOL ApplyAcrylic(HWND hwnd);
BOOL ApplyTabbed(HWND hwnd);
BOOL ApplyNoMaterial(HWND hwnd);
void SetWindowBlur(HWND hWnd);
BOOL TrayIcon(HWND hWnd, HINSTANCE hInst);
void DisableMaximizeButton(HWND hwnd);
BOOL RtlGetVersion(OSVERSIONINFOEX* os);
