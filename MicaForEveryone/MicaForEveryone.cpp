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
// processes messages related to UAH / custom menubar drawing.
// return true if handled, false to continue with normal processing in your wndproc
bool UAHDarkModeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lr)
{
    switch (message)
    {
    case WM_UAHDRAWMENU:
    {
        UAHMENU* pUDM = (UAHMENU*)lParam;
        RECT rc = { 0 };

        // get the menubar rect
        {
            MENUBARINFO mbi = { sizeof(mbi) };
            GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi);

            RECT rcWindow;
            GetWindowRect(hWnd, &rcWindow);

            // the rcBar is offset by the window rect
            rc = mbi.rcBar;
            OffsetRect(&rc, -rcWindow.left, -rcWindow.top);

            rc.top -= 1;
        }

        if (!g_menuTheme) {
            g_menuTheme = OpenThemeData(hWnd, L"Menu");
        }

        DrawThemeBackground(g_menuTheme, pUDM->hdc, MENU_POPUPITEM, MPI_NORMAL, &rc, nullptr);

        return true;
    }
    case WM_UAHDRAWMENUITEM:
    {
        UAHDRAWMENUITEM* pUDMI = (UAHDRAWMENUITEM*)lParam;

        // get the menu item string
        wchar_t menuString[256] = { 0 };
        MENUITEMINFO mii = { sizeof(mii), MIIM_STRING };
        {
            mii.dwTypeData = menuString;
            mii.cch = (sizeof(menuString) / 2) - 1;

            GetMenuItemInfo(pUDMI->um.hmenu, pUDMI->umi.iPosition, TRUE, &mii);
        }

        // get the item state for drawing

        DWORD dwFlags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;

        int iTextStateID = 0;
        int iBackgroundStateID = 0;
        {
            if ((pUDMI->dis.itemState & ODS_INACTIVE) | (pUDMI->dis.itemState & ODS_DEFAULT)) {
                // normal display
                iTextStateID = MPI_NORMAL;
                iBackgroundStateID = MPI_NORMAL;
            }
            if (pUDMI->dis.itemState & ODS_HOTLIGHT) {
                // hot tracking
                iTextStateID = MPI_HOT;
                iBackgroundStateID = MPI_HOT;
            }
            if (pUDMI->dis.itemState & ODS_SELECTED) {
                // clicked -- MENU_POPUPITEM has no state for this, though MENU_BARITEM does
                iTextStateID = MPI_HOT;
                iBackgroundStateID = MPI_HOT;
            }
            if ((pUDMI->dis.itemState & ODS_GRAYED) || (pUDMI->dis.itemState & ODS_DISABLED)) {
                // disabled / grey text
                iTextStateID = MPI_DISABLED;
                iBackgroundStateID = MPI_DISABLED;
            }
            if (pUDMI->dis.itemState & ODS_NOACCEL) {
                dwFlags |= DT_HIDEPREFIX;
            }
        }

        if (!g_menuTheme) {
            g_menuTheme = OpenThemeData(hWnd, L"Menu");
        }

        DrawThemeBackground(g_menuTheme, pUDMI->um.hdc, MENU_POPUPITEM, iBackgroundStateID, &pUDMI->dis.rcItem, nullptr);
        DrawThemeText(g_menuTheme, pUDMI->um.hdc, MENU_POPUPITEM, iTextStateID, menuString, mii.cch, dwFlags, 0, &pUDMI->dis.rcItem);

        return true;
    }
    case WM_THEMECHANGED:
    {
        if (g_menuTheme) {
            CloseThemeData(g_menuTheme);
            g_menuTheme = nullptr;
        }
        // continue processing in main wndproc
        return false;
    }
    default:
        return false;
    }
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
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDR_MENU1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MICAFOREVERYONE));

    return RegisterClassExW(&wcex);
}
BOOL TrayIcon(HWND hWnd)
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

   SetPreferredAppMode = (fnSetPreferredAppMode)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
   RefreshImmersiveColorPolicyState = reinterpret_cast<fnRefreshImmersiveColorPolicyState>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(104)));
   AllowDarkModeForWindow = reinterpret_cast<fnAllowDarkModeForWindow>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133)));
   SetPreferredAppMode(PreferredAppMode::ForceDark);
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 840, 470, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd)
   {
      return FALSE;
   }
   TrayIcon(hWnd);
   DisableMaximizeButton(hWnd);
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
            if (IDM_GUI == menuItemId)
            {
                ShowWindow(hwnd, SW_SHOW);
            }
            
        DestroyMenu(hMenu);
    }
}

VOID CALLBACK WinEventProcCallback(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    if (dwEvent == EVENT_OBJECT_CREATE)
    {
        if (IsWindow(hwnd))
        {
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
    HBRUSH brush = CreateSolidBrush(darkBkColor);
    RtlGetVersion(&os);
    static UINT s_uTaskbarRestart; 
    LRESULT lr = 0;
    if (UAHDarkModeWndProc(hWnd, message, wParam, lParam, &lr)) {
        return lr;
    }
    switch (message)
    {
    case WM_CREATE:
    {
        AllowDarkModeForWindow(hWnd, true);
        nice = TRUE;
        DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &nice, sizeof nice);
        Title = CreateWindowW(L"static", L"TITLE",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            20, 20, 450, 35,
            hWnd, (HMENU)1, NULL, NULL);
        ShowWindow(Title, SW_SHOW);
        SetWindowText(Title, L"Settings");
        hFont = CreateFontW(35, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        SendMessage(Title, WM_SETFONT, WPARAM(hFont), TRUE);
        DwmExtendFrameIntoClientArea(hWnd, &margins);
        if (os.dwBuildNumber == 22000)
        {
            nice = TRUE;
            DwmSetWindowAttribute(hWnd, DWMWA_MICA_EFFECT, &nice, sizeof nice);
        }
        if (os.dwBuildNumber >= 22523)
        {
            int value = 2;
            DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &value, sizeof value);
        }
        hEvent = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, NULL,
            WinEventProcCallback, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNTHREAD);
        s_uTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
    }
        break;
    case WM_CLOSE:
        ShowWindow(hWnd, SW_HIDE);
        break;
    case WM_ERASEBKGND:
        SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);
    case WM_USER_SHELLICON:
        // systray msg callback 
        switch (LOWORD(lParam))
        {
        case WM_RBUTTONDOWN:
            GetCursorPos(&ok);
            EnumWindows(hwndcallback, reinterpret_cast<LPARAM>(&hwndlist));
            ShowContextMenu(hWnd, ok);
            return TRUE;
        case WM_LBUTTONDOWN:
            ShowWindow(hWnd, SW_SHOW);
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
    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        SetTextColor(hdc, darkTextColor);
        SetBkColor(hdc, darkBkColor);
        if (!hbrBkgnd)
            hbrBkgnd = CreateSolidBrush(darkBkColor);
        return reinterpret_cast<INT_PTR>(hbrBkgnd);
    }
        break;
    case WM_DESTROY:
        if (hEvent) 
            UnhookWinEvent(hEvent);
        PostQuitMessage(0);
        break;
    default:
        if (message == s_uTaskbarRestart) {
            TrayIcon(hWnd);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        SetWindowTheme(GetDlgItem(hDlg, IDOK), L"Explorer", nullptr);
        SendMessageW(hDlg, WM_THEMECHANGED, 0, 0);
        HFONT hOrigFont = (HFONT)SendMessage(GetDlgItem(hDlg, IDC_STATIC_LINK), WM_GETFONT, 0, 0);
        LOGFONT lf;
        GetObject(hOrigFont, sizeof(lf), &lf);
        lf.lfUnderline = TRUE;
        HFONT hFont = CreateFontIndirect(&lf);
        SendMessage(GetDlgItem(hDlg, IDC_STATIC_LINK), WM_SETFONT, WPARAM(hFont), TRUE);
    }
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        switch (LOWORD(wParam))
        {
        case IDC_STATIC_LINK:
            ShellExecute(NULL, L"open", L"https://github.com/minusium/MicaForEveryone", NULL, NULL, SW_SHOWNORMAL);
            break;
        }
        break;
    case WM_CTLCOLORDLG:
        return (INT_PTR)CreateSolidBrush(darkBkColor);
    case WM_CTLCOLORSTATIC:
        {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            SetTextColor(hdc, darkTextColor);
            SetBkColor(hdc, darkBkColor);
            if (!hbrBkgnd)
                hbrBkgnd = CreateSolidBrush(darkBkColor);
            if ((HWND)lParam == GetDlgItem(hDlg, IDC_STATIC_LINK))
            {
                SetTextColor((HDC)wParam, RGB(166, 216, 255));
            }
            return reinterpret_cast<INT_PTR>(hbrBkgnd);

        }
        break;
    case WM_SETCURSOR:
    {
        if ((HWND)(wParam) == ::GetDlgItem(hDlg, IDC_STATIC_LINK))
        {
            HCURSOR hCursorHelp = ::LoadCursor(NULL, IDC_HAND);
            ::SetCursor(hCursorHelp);
            ::SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
            return TRUE;
        }
        return FALSE;
    }
    case WM_DESTROY:
        if (hbrBkgnd)
        {
            DeleteObject(hbrBkgnd);
            hbrBkgnd = nullptr;
        }
        break;
    case WM_THEMECHANGED:
        {
            nice = TRUE;
            AllowDarkModeForWindow(hDlg, true);
            DwmSetWindowAttribute(hDlg, DWMWA_USE_IMMERSIVE_DARK_MODE, &nice, sizeof nice);
            HWND hButton = GetDlgItem(hDlg, IDOK);
            AllowDarkModeForWindow(hButton, true);
            SendMessageW(hButton, WM_THEMECHANGED, 0, 0);
            UpdateWindow(hDlg);
        }
    }
    return (INT_PTR)FALSE;
}
