// MicaForEveryone.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "MicaForEveryone.h"
#include <string>

// functions
#define MAX_LOADSTRING 100
#define	WM_USER_SHELLICON WM_USER + 1
#define DWMWA_MICA_EFFECT 1029
#define DWMWA_SYSTEMBACKDROP_TYPE 38

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
NOTIFYICONDATA nidApp;
BOOL Extend = FALSE;
BOOL Dark = FALSE;
BOOL Light = FALSE;
BOOL Mica = FALSE;
BOOL DefaultBack = TRUE;
BOOL Acrylic = FALSE;
BOOL Tabbed = FALSE;
BOOL DefaultCol = TRUE;
DWORD nice = TRUE;
HWINEVENTHOOK hEvent;
std::vector<HWND> hwndlist;
MARGINS margins = { -1 };
MENUITEMINFO mi = { 0 };
OSVERSIONINFOEX os;
UINT menuItemId = 0;
POINT ok;
fnSetWindowCompositionAttribute SetWindowCompositionAttribute;
fnSetPreferredAppMode SetPreferredAppMode;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

typedef void (WINAPI* RtlGetVersion_FUNC) (OSVERSIONINFOEXW*);

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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MICAFOREVERYONE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MICAFOREVERYONE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MICAFOREVERYONE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MICAFOREVERYONE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HMODULE hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable
   HICON hMainIcon;
   // SetWindowCompositionAttribute = (fnSetWindowCompositionAttribute)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
   SetPreferredAppMode = (fnSetPreferredAppMode)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
   SetPreferredAppMode(ForceDark);
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd)
   {
      return FALSE;
   }
   hMainIcon = LoadIcon(hInstance, (LPCTSTR)MAKEINTRESOURCE(IDI_MICAFOREVERYONE));
   nidApp.cbSize = sizeof(NOTIFYICONDATA);
   nidApp.hWnd = (HWND)hWnd;
   nidApp.uID = IDI_MICAFOREVERYONE;
   nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
   nidApp.hIcon = hMainIcon;
   nidApp.uCallbackMessage = WM_USER_SHELLICON;
   nidApp.uVersion = NOTIFYICON_VERSION_4;
   LoadString(hInstance, IDS_APPTOOLTIP, nidApp.szTip, MAX_LOADSTRING);
   Shell_NotifyIcon(NIM_ADD, &nidApp);
   return TRUE;
}
BOOL CALLBACK hwndcallback(HWND hwnd, LPARAM lParam) {
    std::vector<HWND>& hwnds =
         *reinterpret_cast<std::vector<HWND>*>(lParam);
    hwnds.push_back(hwnd);
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
void ShowContextMenu(HWND hwnd, POINT pt)
{
    HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDC_MICAFOREVERYONE));
    if (hMenu)
    {
        
        HMENU hSubMenu = GetSubMenu(hMenu, 0);
        if (hSubMenu)
        {
            SetForegroundWindow(hwnd);
            if (os.dwBuildNumber < 22000)
            {
                EnableMenuItem(hSubMenu, IDM_MICA, MF_GRAYED);
                EnableMenuItem(hSubMenu, IDM_ACRYLIC, MF_ENABLED);
                EnableMenuItem(hSubMenu, IDM_TABBED, MF_GRAYED);
            }
            if (os.dwBuildNumber == 22000)
            {
                EnableMenuItem(hSubMenu, IDM_MICA, MF_ENABLED);
                EnableMenuItem(hSubMenu, IDM_ACRYLIC, MF_GRAYED);
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
                SetMenuItemInfo(hSubMenu, IDM_DEFAULTCOL, FALSE, &mi);
            }
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
                    Extend = FALSE;
                else
                {
                    Extend = TRUE;
                    for (const HWND& hwnds : hwndlist)
                    {
                        DwmExtendFrameIntoClientArea(hwnds, &margins);
                    }
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
                Dark = TRUE;
                Light = FALSE;
                nice = TRUE;
                for (const HWND& hwnds : hwndlist)
                {
                    DwmSetWindowAttribute(hwnds, DWMWA_USE_IMMERSIVE_DARK_MODE, &nice, sizeof nice);
                }
            }
            if (IDM_LIGHT == menuItemId)
            {
                DefaultCol = FALSE;
                Light = TRUE;
                Dark = FALSE;
                nice = FALSE;
                for (const HWND& hwnds : hwndlist)
                {
                    DwmSetWindowAttribute(hwnds, DWMWA_USE_IMMERSIVE_DARK_MODE, &nice, sizeof nice);
                }
            }
            if (IDM_DEFAULTCOL == menuItemId)
            {
                DefaultCol = TRUE;
                Light = FALSE;
                Dark = FALSE;
            }
            if (IDM_MICA == menuItemId)
            {
                Mica = TRUE;
                DefaultBack = FALSE;
                Acrylic = FALSE;
                Tabbed = FALSE;
                if (os.dwBuildNumber == 22000)
                {
                    nice = TRUE;
                    for (const HWND& hwnds : hwndlist)
                    {
                        DwmSetWindowAttribute(hwnds, DWMWA_MICA_EFFECT, &nice, sizeof nice);
                    }
                }
                if (os.dwBuildNumber >= 22523)
                {
                    int value = 2;
                    for (const HWND& hwnds : hwndlist)
                    {
                        DwmSetWindowAttribute(hwnds, DWMWA_SYSTEMBACKDROP_TYPE, &value, sizeof value);
                    }
                }
            }
            if (IDM_ACRYLIC == menuItemId)
            {
                DefaultBack = FALSE;
                Mica = FALSE;
                Acrylic = TRUE;
                Tabbed = FALSE;
                int value = 3;

                if (os.dwBuildNumber < 22000)
                {
                    for (const HWND& hwnds : hwndlist)
                    {
                        SetWindowLongPtr(hwnds, GWL_EXSTYLE, GetWindowLong(hwnds, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_NOREDIRECTIONBITMAP);
                        SetWindowBlur(hwnds);
                    }
                }
                else if (os.dwBuildNumber >= 22523)
                {
                    for (const HWND& hwnds : hwndlist)
                    {
                        DwmSetWindowAttribute(hwnds, DWMWA_SYSTEMBACKDROP_TYPE, &value, sizeof value);
                    }
                }
            }
            if (IDM_TABBED == menuItemId)
            {
                Mica = FALSE;
                DefaultBack = FALSE;
                Acrylic = FALSE;
                Tabbed = TRUE;
                int value = 4;
                for (const HWND& hwnds : hwndlist)
                {
                    DwmSetWindowAttribute(hwnds, DWMWA_SYSTEMBACKDROP_TYPE, &value, sizeof value);
                }
            }
            if (IDM_DEFAULTBACK == menuItemId)
            {
                Mica = FALSE;
                DefaultBack = TRUE;
                Acrylic = FALSE;
                Tabbed = FALSE;
                int value = 1;
                for (const HWND& hwnds : hwndlist)
                {
                    DwmSetWindowAttribute(hwnds, DWMWA_SYSTEMBACKDROP_TYPE, &value, sizeof value);
                }
            }
            
        DestroyMenu(hMenu);
    }
}

VOID CALLBACK WinEventProcCallback(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    if (dwEvent == EVENT_OBJECT_CREATE) {
        if (DefaultCol) 
        {

        }
        if (Light) 
        {
            nice = FALSE;
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &nice, sizeof nice);
        }
        if (Dark) 
        {
            nice = TRUE;
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &nice, sizeof nice);
        }
        if (DefaultBack) 
        {
            int value = 1;
            DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &value, sizeof value);
        }
        if (Mica) 
        {
            if (os.dwBuildNumber == 22000)
            {
                nice = TRUE;
                DwmSetWindowAttribute(hwnd, DWMWA_MICA_EFFECT, &nice, sizeof nice);
            }
            if (os.dwBuildNumber >= 22523)
            {
                int value = 2;
                DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &value, sizeof value);
            }
        }
        if (Acrylic) 
        {
            if (os.dwBuildNumber < 22000)
            {
                SetWindowLongPtr(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_NOREDIRECTIONBITMAP);
                SetWindowBlur(hwnd);
            }
            else if (os.dwBuildNumber >= 22523)
            {
                int value = 3;
                DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &value, sizeof value);
            }
        }
        if (Tabbed)
        {
            int value = 4;
            DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &value, sizeof value);
        }
    }
}
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RtlGetVersion(&os);
    switch (message)
    {
    case WM_CREATE: 
        hEvent = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, NULL,
            WinEventProcCallback, 0, 0, WINEVENT_OUTOFCONTEXT);
        break;
    case WM_USER_SHELLICON:
        // systray msg callback 
        switch (LOWORD(lParam))
        {
        case WM_RBUTTONDOWN:
            GetCursorPos(&ok);
            EnumWindows(hwndcallback, reinterpret_cast<LPARAM>(&hwndlist));
            ShowContextMenu(hWnd, ok);
            return TRUE;
        }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                Shell_NotifyIcon(NIM_DELETE, &nidApp);
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_DESTROY:
        if (hEvent) UnhookWinEvent(hEvent);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    BOOL ok = TRUE;
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
