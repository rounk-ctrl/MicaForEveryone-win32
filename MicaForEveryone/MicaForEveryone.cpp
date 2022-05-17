// MicaForEveryone.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "MicaForEveryone.h"
#include "MicaForEveryoneHelper.h"
#include <string>


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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

	BOOL bRet;
	MSG msg;

	// Main message loop:
	while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			// handle the error and possibly exit
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

    return (int) msg.wParam;
}

int DarkThemeEnabled;
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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   SetPreferredAppMode = (fnSetPreferredAppMode)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
   RefreshImmersiveColorPolicyState = reinterpret_cast<fnRefreshImmersiveColorPolicyState>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(104)));
   AllowDarkModeForWindow = reinterpret_cast<fnAllowDarkModeForWindow>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133)));
   FreeLibrary(hUxtheme);
   DarkThemeEnabled = IsExplorerDarkTheme();
   SetPreferredAppMode(PreferredAppMode::AllowDark);
   RefreshImmersiveColorPolicyState();
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 840, 470, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd)
   {
      return FALSE;
   }
   TrayIcon(hWnd, hInst);
   DisableMaximizeButton(hWnd);
   ApplyDarkTitleBar(hWnd, DarkThemeEnabled);
   return TRUE;
}
BOOL CALLBACK hwndcallback(HWND hwnd, LPARAM lParam) {
    std::vector<HWND>& hwnds =
        *reinterpret_cast<std::vector<HWND>*>(lParam);
    hwnds.push_back(hwnd);
    return TRUE;
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
            else if (os.dwBuildNumber == 22000)
            {
                EnableMenuItem(hSubMenu, IDM_MICA, MF_ENABLED);
                EnableMenuItem(hSubMenu, IDM_ACRYLIC, MF_ENABLED);
                EnableMenuItem(hSubMenu, IDM_TABBED, MF_GRAYED);
            }
            else if (os.dwBuildNumber >= 22494)
            {
                EnableMenuItem(hSubMenu, IDM_MICA, MF_GRAYED);
                EnableMenuItem(hSubMenu, IDM_ACRYLIC, MF_GRAYED);
                EnableMenuItem(hSubMenu, IDM_TABBED, MF_GRAYED);
            }
            else if (os.dwBuildNumber >= 22523)
            {
                EnableMenuItem(hSubMenu, IDM_MICA, MF_ENABLED);
                EnableMenuItem(hSubMenu, IDM_ACRYLIC, MF_ENABLED);
                EnableMenuItem(hSubMenu, IDM_TABBED, MF_ENABLED);
            }
			// global
			mi.cbSize = sizeof(MENUITEMINFO);
			mi.fMask = MIIM_STATE;
			mi.fState = MF_CHECKED;

            if (Extend)
            {
                SetMenuItemInfo(hSubMenu, IDM_EXTEND, FALSE, &mi);                
            }
            else if (Dark) 
            {
                SetMenuItemInfo(hSubMenu, IDM_DARK, FALSE, &mi);
            }
            else if (Light)
            {
                SetMenuItemInfo(hSubMenu, IDM_LIGHT, FALSE, &mi);
            }
            else if (System)
            {
                SetMenuItemInfo(hSubMenu, IDM_SYS, FALSE, &mi);
            }
            else if (Mica)
            {
                SetMenuItemInfo(hSubMenu, IDM_MICA, FALSE, &mi);
            }
            else if (DefaultBack)
            {
                SetMenuItemInfo(hSubMenu, IDM_DEFAULTBACK, FALSE, &mi);
            }
            else if (Acrylic)
            {
                SetMenuItemInfo(hSubMenu, IDM_ACRYLIC, FALSE, &mi);
            }
            else if (Tabbed)
            {
                SetMenuItemInfo(hSubMenu, IDM_TABBED, FALSE, &mi);
            }
            else if (DefaultCol)
            {
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
			switch (menuItemId)
			{
				case IDM_EXTEND:
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
					break;
				}
				case IDM_EXIT:
				{
					Shell_NotifyIcon(NIM_DELETE, &nidApp);
					PostQuitMessage(0);
					break;

				}
				case IDM_ABOUT:
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
					break;
				}
				case IDM_DARK:
				{
					DefaultCol = FALSE;
					Dark = TRUE;
					Light = FALSE;
					System = FALSE;
					for (const HWND& hwnds : hwndlist)
					{
						ApplyDarkTitleBar(hwnds, TRUE);
					}
					break;
				}
				case IDM_LIGHT:
				{
					DefaultCol = FALSE;
					Light = TRUE;
					Dark = FALSE;
					System = FALSE;
					for (const HWND& hwnds : hwndlist)
					{
						ApplyDarkTitleBar(hwnds, FALSE);
					}
					break;
				}
				case IDM_SYS:
				{
					DefaultCol = FALSE;
					Light = FALSE;
					Dark = FALSE;
					System = TRUE;
					for (const HWND& hwnds : hwndlist)
					{
						ApplyDarkTitleBar(hwnds, DarkThemeEnabled);
					}
					break;
				}
				case IDM_DEFAULTCOL:
				{
					DefaultCol = TRUE;
					Light = FALSE;
					Dark = FALSE;
					System = FALSE;
					break;
				}
				case IDM_MICA:
				{
					Mica = TRUE;
					DefaultBack = FALSE;
					Acrylic = FALSE;
					Tabbed = FALSE;
					for (const HWND& hwnds : hwndlist)
					{
						ApplyMica(hwnds);
					}
					break;
				}
				case IDM_ACRYLIC:
				{
					DefaultBack = FALSE;
					Mica = FALSE;
					Acrylic = TRUE;
					Tabbed = FALSE;
					for (const HWND& hwnds : hwndlist)
					{
						ApplyAcrylic(hwnds);
					}
					break;
				}
				case IDM_TABBED:
				{
					Mica = FALSE;
					DefaultBack = FALSE;
					Acrylic = FALSE;
					Tabbed = TRUE;
					for (const HWND& hwnds : hwndlist)
					{
						ApplyTabbed(hwnds);
					}
					break;
				}
				case IDM_DEFAULTBACK:
				{
					Mica = FALSE;
					DefaultBack = TRUE;
					Acrylic = FALSE;
					Tabbed = FALSE;
					break;
				}
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
            if (Light)
            {
                ApplyDarkTitleBar(hwnd, FALSE);
            }
            else if (Dark)
            {
                ApplyDarkTitleBar(hwnd, TRUE);
            }
            else if (System)
            {
                ApplyDarkTitleBar(hwnd, DarkThemeEnabled);
            }
            else if (DefaultBack)
            {
                SetSystemBackdropType(hwnd, SystemBackdropType::Auto);
            }
            else if (Mica)
            {
                ApplyMica(hwnd);
            }
            else if (Acrylic)
            {
                ApplyAcrylic(hwnd);
            }
            else if (Tabbed)
            {
                ApplyTabbed(hwnd);
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
    HBRUSH brush = CreateSolidBrush(DarkThemeEnabled ? darkBkColor : lightBkColor);
    RtlGetVersion(&os);
    static UINT s_uTaskbarRestart; 
    switch (message)
    {
    case WM_CREATE:
    {
        hEvent = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, NULL,
            WinEventProcCallback, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNTHREAD);
        s_uTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
		break;
    }
    case WM_USER_SHELLICON:
        // systray msg callback 
        switch (LOWORD(lParam))
        {
			case WM_RBUTTONDOWN:
				GetCursorPos(&ok);
				EnumWindows(hwndcallback, reinterpret_cast<LPARAM>(&hwndlist));
				ShowContextMenu(hWnd, ok);
				break;
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
            DwmExtendFrameIntoClientArea(hDlg, &margins);
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
