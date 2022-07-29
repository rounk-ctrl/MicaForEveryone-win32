// MicaForEveryone.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "MicaForEveryone.h"
#include "MicaForEveryoneHelper.h"
#include "ContextMenuHandler.h"


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

// some functions used for dark mode
fnSetPreferredAppMode SetPreferredAppMode;
fnAllowDarkModeForWindow AllowDarkModeForWindow;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MICAFOREVERYONE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	UpdateConfig();

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
        return FALSE;

    MSG msg;
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
		TranslateMessage(&msg);
        DispatchMessage(&msg);
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
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MICAFOREVERYONE));

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
int DarkThemeEnabled;
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   SetPreferredAppMode = (fnSetPreferredAppMode)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
   AllowDarkModeForWindow = reinterpret_cast<fnAllowDarkModeForWindow>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133)));
   FreeLibrary(hUxtheme);
   DarkThemeEnabled = IsExplorerDarkTheme();
   SetPreferredAppMode(PreferredAppMode::AllowDark);
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 840, 470, nullptr, nullptr, hInstance, nullptr);
   
   if (!hWnd)
   {
      return FALSE;
   }
   TrayIcon(hWnd, hInst);
   DisableMaximizeButton(hWnd);
   return TRUE;
}
BOOL CALLBACK hwndcallback(HWND hwnd, LPARAM lParam) {
    std::vector<HWND>& hwnds =
        *reinterpret_cast<std::vector<HWND>*>(lParam);
    hwnds.push_back(hwnd);
    return TRUE;
}

VOID CALLBACK WinEventProcCallback(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    if (dwEvent == EVENT_OBJECT_CREATE)
    {
        if (IsWindow(hwnd))
        {
			if (SysCol)
			{
				DarkThemeEnabled = IsExplorerDarkTheme();
				ApplyDarkTitleBar(hwnd, DarkThemeEnabled);
			}
            if (Light)
                ApplyDarkTitleBar(hwnd, FALSE);
            if (Dark)
                ApplyDarkTitleBar(hwnd, TRUE);
			if (None)
				SetSystemBackdropType(hwnd, SystemBackdropType::None);
            if (Mica)
                ApplyMica(hwnd);
            if (Acrylic)
                ApplyAcrylic(hwnd);
            if (Tabbed)
                ApplyTabbed(hwnd);
			if (Square)
				SetWindowRoundPreference(hwnd, DWMWCP_DONOTROUND);
			if (Round)
				SetWindowRoundPreference(hwnd, DWMWCP_ROUND);
			if (SRound)
				SetWindowRoundPreference(hwnd, DWMWCP_ROUNDSMALL);
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
    switch (message)
    {
    case WM_CREATE:
    {
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
			DarkThemeEnabled = IsExplorerDarkTheme();
			UpdateConfig();
            ShowContextMenu(hWnd, ok, hInst, DarkThemeEnabled);
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
        HWND hWnd = (HWND)lParam;
        SetTextColor(hdc, darkTextColor);
        SetBkColor(hdc, darkBkColor);
        if (!hbrBkgnd)
            hbrBkgnd = CreateSolidBrush(darkBkColor);
        return reinterpret_cast<INT_PTR>(hbrBkgnd);
    }
    break;
    case WM_CTLCOLORLISTBOX:
    {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        SetTextColor(hdc, darkTextColor);
        SetBkColor(hdc, darkBkColor);
        if (!hbrBkgnd)
            hbrBkgnd = CreateSolidBrush(darkBkColor);
        return reinterpret_cast<INT_PTR>(hbrBkgnd);
    }
    break;
    case WM_CTLCOLOREDIT: 
    {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        SetBkMode(hdc, OPAQUE);
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
            TrayIcon(hWnd, hInst);
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
        ApplyMica(hDlg);
        if (DarkThemeEnabled)
        {
            SetWindowTheme(GetDlgItem(hDlg, IDOK), L"Explorer", nullptr);
			if (os.dwBuildNumber >= 17763)
			{
				DwmExtendFrameIntoClientArea(hDlg, &margins);
			}
            SendMessageW(hDlg, WM_THEMECHANGED, 0, 0);
        }
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
        if (DarkThemeEnabled)
        {
            return (INT_PTR)CreateSolidBrush(darkBkColor);
        }
    case WM_CTLCOLORSTATIC:
    {
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_STATIC_LINK))
        {
            SetTextColor((HDC)wParam, RGB(166, 216, 255));
        }
        if (DarkThemeEnabled)
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
        if (DarkThemeEnabled)
        {
            AllowDarkModeForWindow(hDlg, true);
            ApplyDarkTitleBar(hDlg, TRUE);
            HWND hButton = GetDlgItem(hDlg, IDOK);
            AllowDarkModeForWindow(hButton, true);
            SendMessageW(hButton, WM_THEMECHANGED, 0, 0);
            UpdateWindow(hDlg);
        }
    }
    }
    return (INT_PTR)FALSE;
}
