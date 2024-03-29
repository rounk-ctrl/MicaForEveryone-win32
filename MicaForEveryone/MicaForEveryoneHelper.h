#pragma once
#include "framework.h"
#include "resource.h"

typedef void (WINAPI* RtlGetVersion_FUNC) (OSVERSIONINFOEXW*);

#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#define DWMWA_MICA_EFFECT 1029
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#define MAX_LOADSTRING 100
#define	WM_USER_SHELLICON WM_USER + 1
#define DWMWA_WINDOW_CORNER_PREFERENCE 33

enum class SystemBackdropType 
{
	Auto,
	None,
	Mica,
	Acrylic,
	Tabbed
};

typedef enum {
	DWMWCP_DEFAULT = 0,
	DWMWCP_DONOTROUND = 1,
	DWMWCP_ROUND = 2,
	DWMWCP_ROUNDSMALL = 3
} DWM_WINDOW_CORNER_PREFERENCE;

class Window
{
private:
	HWND m_hwnd;
	DWORD m_pid;
	std::wstring m_name;
public:
	HWND GetHwnd() const { return m_hwnd; }
	std::wstring GetProcName() const { return m_name; }
	DWORD GetPid() const { return m_pid; }
	Window(HWND hwnd, DWORD pid, std::wstring name)
		: m_hwnd(hwnd), m_pid(pid), m_name(name)
	{}
};

extern BOOL Extend;
extern BOOL Dark;
extern BOOL Light;
extern BOOL Mica;
extern BOOL DefaultBack;
extern BOOL Acrylic;
extern BOOL Tabbed;
extern BOOL DefaultCol;
extern BOOL SysCol;
extern NOTIFYICONDATA nidApp;
extern HWINEVENTHOOK hEvent;
extern MARGINS margins;
extern MENUITEMINFO mi;
extern OSVERSIONINFOEX os;
extern UINT menuItemId;
extern HICON hMainIcon;
extern POINT ok;
extern HKEY hKeyPersonalization;
extern BOOL None;
extern BOOL DefCor;
extern BOOL Square;
extern BOOL Round;
extern BOOL SRound;
extern std::vector<TCHAR*> procnamelist;

extern std::vector<Window> hwndlist;
extern std::vector<std::string> rulelist;
extern std::vector<std::string> processlist;


BOOL ApplyDarkTitleBar(HWND hwnd, BOOL mode);
BOOL CobaltMicaEffect(HWND hwnd, BOOL allow);
void SetSystemBackdropType(HWND hwnd, SystemBackdropType value);
BOOL ApplyMica(HWND hwnd);
BOOL ApplyAcrylic(HWND hwnd);
BOOL ApplyTabbed(HWND hwnd);
BOOL ApplyNoMaterial(HWND hwnd);
BOOL ApplyDefaultMaterial(HWND hwnd);
void SetWindowBlur(HWND hWnd);
BOOL TrayIcon(HWND hWnd, HINSTANCE hInst);
void DisableMaximizeButton(HWND hwnd);
BOOL RtlGetVersion(OSVERSIONINFOEX* os);
void SetWindowRoundPreference(HWND hwnd, DWM_WINDOW_CORNER_PREFERENCE value);
void UpdateConfig(std::wstring name);
void MatchAndApplyRule(int DarkThemeEnabled, int type, HWND hwnd);
int IsExplorerDarkTheme();
void EnableDebugPriv();
