#include "mag.h"
// Global variables and strings.
HINSTANCE           hInst;
const TCHAR         WindowClassName[] = TEXT("MagnifierWindow");
const TCHAR         WindowTitle[] = TEXT("Screen Magnifier Sample");
const UINT          timerInterval = 16;//15; // close to the refresh rate @60hz
const char ClassName[] = "MainWindowClass";
HWND                hwndMag;
HWND                hwndHost;
HWND    hWndSetup; // settings window
HWND	hwndColor;
RECT                magWindowRect;
RECT                hostWindowRect;
_In_ int       nCmd;
bool ShowMag = true;
bool Invert = true;
bool Gray = false;
bool HighContrast = false;
bool Quit = false;
bool runmag = true;
bool showOptions = false;
bool zoomChanged = false;
bool colorChanged = false;
bool showColorMatrix = false;
float MAGFACTOR = 2.2f;
int LensSize = 0;  // 0 = S, 1 = M, 2 = L

				   // Forward declarations.
ATOM                RegisterHostWindowClass(HINSTANCE hInstance);
BOOL                SetupMagnifier(HINSTANCE hinst);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void CALLBACK       UpdateMagWindow(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
BOOL                isFullScreen = FALSE;
HHOOK Mousehook; //global declaration
HHOOK KeyHook;
HWND hWndButton1;
HWND hWndButton2;
HWND hWndButton3;
HWND hWndButton4;
HWND hWndButton5;
HWND hWndButton6;
HWND hWndButton7;
HWND hWndButton8;
HWND hWndButton9;
HWND hWndText;
HWND hWndText2;
HWND hWndOk;
HWND contrastControl;
HWND scroll1;
HWND hWndStatus;
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK colorWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int RunMag(_In_ int       nCmdShow);
void Restart(_In_ int       nCmdShow);
void SettingsWindow(HINSTANCE  hInstance)
{
	// Settings window
	WNDCLASSEX    wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0)); //(HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "SettingsWin";

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "Error", MB_OK | MB_ICONERROR);
	}
	
	hWndSetup = CreateWindowEx( // Settings window
		WS_EX_CLIENTEDGE,
		"SettingsWin",
		"                                        Select color mode & zoom level",
		WS_OVERLAPPEDWINDOW,
		500,
		300,
		600,
		640,
		NULL,
		NULL,
		hInstance,
		NULL);
	if (!hWndSetup)
	{
		MessageBox(NULL, "Window Creation Failed.", "Error", MB_OK | MB_ICONERROR);
		//return 0;
	}
	ShowWindow(hWndSetup, SW_SHOW);
	UpdateWindow(hWndSetup);
	MSG    Msg;
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
		
	}

}

// Callback procedure for Settings window
LRESULT CALLBACK WndProc(
	HWND    hWnd,
	UINT    Msg,
	WPARAM  wParam,
	LPARAM  lParam)
{
	//bool leaveOpen = true;
	static HFONT s_hFont = NULL;
	switch (Msg)
	{
	case WM_CREATE:
	{
		char* fontName = "Virgil";
		const long nFontSize = 14;

		HDC hdc = GetDC(hWnd);
		LOGFONT logFont = { 0 };
		logFont.lfHeight = -MulDiv(nFontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		logFont.lfWeight = FW_BOLD;
		strcpy_s(logFont.lfFaceName, fontName);
		s_hFont = CreateFont(36, 0, 0, 0, FW_NORMAL, 0,
			0, 0, 0, 0, 0, PROOF_QUALITY, 0, "Cambria");
		ReleaseDC(hWnd, hdc);


		HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		/*hWndButton1 = CreateWindowEx(
			0,
			"SCROLLBAR",
			"Invert Color",
			WS_VISIBLE | WS_CHILD | WS_HSCROLL,
			155,
			20,
			240,
			29,
			hWnd,
			(HMENU)IDB_RADIO1,
			hInstance,
			NULL);*/
		//ShowScrollBar(hwndHost, SB_HORZ, TRUE);

		hWndButton1 = CreateWindowEx(
			0,
			"BUTTON",
			"Invert Color",
			WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
			155,
			20,
			240,
			29,
			hWnd,
			(HMENU)IDB_RADIO1,
			hInstance,
			NULL);

		hWndButton2 = CreateWindowEx(
			0,
			"BUTTON",
			"Normal Color",
			WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
			155,
			50,
			240,
			29,
			hWnd,
			(HMENU)IDB_RADIO2,
			hInstance,
			NULL);
		hWndButton3 = CreateWindowEx(
			0,
			"BUTTON",
			"Grayscale",
			WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
			155,
			80,
			240,
			29,
			hWnd,
			(HMENU)IDB_RADIO3,
			hInstance,
			NULL);
		contrastControl = CreateWindowEx(
			0,
			"BUTTON",
			"High Contrast",
			WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
			155,
			110,
			240,
			29,
			hWnd,
			(HMENU)ID_CONTRAST,
			hInstance,
			NULL);
		hWndButton4 = CreateWindowEx(
			0,
			"BUTTON",
			"2.2x Zoom",
			WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
			155,
			150,
			240,
			29,
			hWnd,
			(HMENU)IDB_RADIO4,
			hInstance,
			NULL);
		hWndButton5 = CreateWindowEx(
			0,
			"BUTTON",
			"3.0x Zoom",
			WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
			155,
			180,
			240,
			29,
			hWnd,
			(HMENU)IDB_RADIO5,
			hInstance,
			NULL);
		hWndButton6 = CreateWindowEx(
			0,
			"BUTTON",
			"4.0x Zoom",
			WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
			155,
			210,
			240,
			29,
			hWnd,
			(HMENU)IDB_RADIO6,
			hInstance,
			NULL);
		hWndButton7 = CreateWindowEx(
			0,
			"BUTTON",
			"S :   850 x 400",
			WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
			155,
			250,
			240,
			29,
			hWnd,
			(HMENU)IDB_RADIO7,
			hInstance,
			NULL);
		hWndButton8 = CreateWindowEx(
			0,
			"BUTTON",
			"M: 1024 x 600",
			WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
			155,
			280,
			240,
			29,
			hWnd,
			(HMENU)IDB_RADIO8,
			hInstance,
			NULL);
		hWndButton9 = CreateWindowEx(
			0,
			"BUTTON",
			"L : 1200 x 800",
			WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
			155,
			310,
			240,
			29,
			hWnd,
			(HMENU)IDB_RADIO9,
			hInstance,
			NULL);
		hWndText = CreateWindowEx(
			0,
			"Edit",
			"To Show/hide magnifier:  Click middle mouse button",
			WS_VISIBLE | WS_CHILD | BS_TEXT,
			5,
			400,
			570,
			50,
			hWnd,
			(HMENU)IDB_RADIO5,
			hInstance,
			NULL);
		hWndText2 = CreateWindowEx(
			0,
			"Edit",
			"To Close: right-click on taskbar icon, select 'close window'",
			WS_VISIBLE | WS_CHILD | BS_TEXT,
			5,
			450,
			570,
			50,
			hWnd,
			(HMENU)IDB_RADIO5,
			hInstance,
			NULL);
		hWndOk = CreateWindow(
			"BUTTON",
			"Ok",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			110,
			520,
			350,
			50,
			hWnd,
			(HMENU)ID_BUTTON,
			hInstance,
			NULL);

		scroll1 = CreateWindowEx(
		WS_EX_LEFT,
		UPDOWN_CLASS,
		"Invert Color",
		WS_CHILDWINDOW | WS_VISIBLE | UDS_ARROWKEYS,
		155,
		600,
		240,
		40,
		hWnd,
		(HMENU)ID_SCROLL,
		hInstance,
		NULL);
		
		//ShowScrollBar(hwndHost, SB_HORZ, TRUE);

		SendMessage(hWndButton1, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
		SendMessage(hWndButton2, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
		SendMessage(hWndButton3, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
		SendMessage(contrastControl, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
		SendMessage(hWndButton4, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
		SendMessage(hWndButton5, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
		SendMessage(hWndButton6, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
		SendMessage(hWndButton7, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
		SendMessage(hWndButton8, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
		SendMessage(hWndButton9, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
		s_hFont = CreateFont(46, 12, 0, 0, FW_NORMAL, 0,
			0, 0, 0, 0, 0, PROOF_QUALITY, 0, "Constantia");
		SendMessage(hWndText, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
		SendMessage(hWndText2, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
		s_hFont = CreateFont(32, 0, 0, 0, FW_NORMAL, 0,
			0, 0, 0, 0, 0, PROOF_QUALITY, 0, "Cambria");
		SendMessage(hWndOk, WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
	}
	// Set default buttons
	SendDlgItemMessage(hWnd, IDB_RADIO1, BM_SETCHECK, 1, 0);
	SendDlgItemMessage(hWnd, IDB_RADIO4, BM_SETCHECK, 1, 0);
	SendDlgItemMessage(hWnd, IDB_RADIO7, BM_SETCHECK, 1, 0);
	break;
	
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_BUTTON:
			switch (HIWORD(wParam))
			{
			case BN_CLICKED: // OK button clicked
				//DeleteObject(s_hFont);
				//DestroyWindow(hwndHost);
				//RunMag(nCmd);
				DestroyWindow(hWnd);
				//PostQuitMessage(0);
				break;
			default:
				break;
			}
			break;
		case IDB_RADIO1:
		{
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if (SendDlgItemMessage(hWnd, IDB_RADIO1, BM_GETCHECK, 0, 0) == 0)
				{
					Invert = true;
					Gray = false;
					HighContrast = false;
					SendDlgItemMessage(hWnd, IDB_RADIO1, BM_SETCHECK, 1, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO2, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO3, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(hWnd, ID_CONTRAST, BM_SETCHECK, 0, 0);
					
				}
				break;
			}
		}
		break;
		case IDB_RADIO2:
		{
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if (SendDlgItemMessage(hWnd, IDB_RADIO2, BM_GETCHECK, 0, 0) == 0)
				{
					Invert = false;
					Gray = false;
					HighContrast = false;
					SendDlgItemMessage(hWnd, IDB_RADIO2, BM_SETCHECK, 1, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO1, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO3, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(hWnd, ID_CONTRAST, BM_SETCHECK, 0, 0);
				}
				break;
			}
		}
		break;

		case IDB_RADIO3:
		{
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if (SendDlgItemMessage(hWnd, IDB_RADIO3, BM_GETCHECK, 0, 0) == 0)
				{
					Gray = true;
					Invert = false;
					HighContrast = false;
					SendDlgItemMessage(hWnd, IDB_RADIO3, BM_SETCHECK, 1, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO1, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO2, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(hWnd, ID_CONTRAST, BM_SETCHECK, 0, 0);
				}
				break;
			}
		}
		break;
		case ID_CONTRAST:
		{
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if (SendDlgItemMessage(hWnd, ID_CONTRAST, BM_GETCHECK, 0, 0) == 0)
				{
					Invert = false;
					Gray = false;
					HighContrast = true;
					SendDlgItemMessage(hWnd, ID_CONTRAST, BM_SETCHECK, 1, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO1, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO3, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO2, BM_SETCHECK, 0, 0);
				}
				break;
			}
		}
		break;

		case IDB_RADIO4:
		{
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if (SendDlgItemMessage(hWnd, IDB_RADIO4, BM_GETCHECK, 0, 0) == 0)
				{
					MAGFACTOR = 2.2f;
					SendDlgItemMessage(hWnd, IDB_RADIO4, BM_SETCHECK, 1, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO5, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO6, BM_SETCHECK, 0, 0);
				}
				break;
			}
		}
		break;
		case IDB_RADIO5:
		{
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if (SendDlgItemMessage(hWnd, IDB_RADIO5, BM_GETCHECK, 0, 0) == 0)
				{
					MAGFACTOR = 3.0f;
					SendDlgItemMessage(hWnd, IDB_RADIO5, BM_SETCHECK, 1, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO6, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO4, BM_SETCHECK, 0, 0);
				}
				break;
			}
		}
		break;
		case IDB_RADIO6:
		{
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if (SendDlgItemMessage(hWnd, IDB_RADIO6, BM_GETCHECK, 0, 0) == 0)
				{
					MAGFACTOR = 4.0f;
					SendDlgItemMessage(hWnd, IDB_RADIO6, BM_SETCHECK, 1, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO4, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO5, BM_SETCHECK, 0, 0);
				}
				break;
			}
		}
		break;
		case IDB_RADIO7:
		{
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if (SendDlgItemMessage(hWnd, IDB_RADIO7, BM_GETCHECK, 0, 0) == 0)
				{
					LensSize = 0;
					SendDlgItemMessage(hWnd, IDB_RADIO7, BM_SETCHECK, 1, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO9, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO8, BM_SETCHECK, 0, 0);
				}
				break;
			}
		}
		break;
		case IDB_RADIO8:
		{
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if (SendDlgItemMessage(hWnd, IDB_RADIO8, BM_GETCHECK, 0, 0) == 0)
				{
					LensSize = 1;
					SendDlgItemMessage(hWnd, IDB_RADIO8, BM_SETCHECK, 1, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO9, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO7, BM_SETCHECK, 0, 0);
				}
				break;
			}
		}
		break;
		case IDB_RADIO9:
		{
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if (SendDlgItemMessage(hWnd, IDB_RADIO9, BM_GETCHECK, 0, 0) == 0)
				{
					LensSize = 2;
					SendDlgItemMessage(hWnd, IDB_RADIO9, BM_SETCHECK, 1, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO8, BM_SETCHECK, 0, 0);
					SendDlgItemMessage(hWnd, IDB_RADIO7, BM_SETCHECK, 0, 0);
				}
				break;
			}
		}
		break;
		}
		return 0;
	}
	break;
	//case WM_PAINT:
	//	break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		DeleteObject(s_hFont);
		break;
	default:
		return (DefWindowProc(hWnd, Msg, wParam, lParam));
	}
	return (DefWindowProc(hWnd, Msg, wParam, lParam));
}
void ShowColor(HINSTANCE hInstance)
{
	// Settings window
	WNDCLASSEX    wColor;
	wColor.cbSize = sizeof(WNDCLASSEX);
	wColor.style = 0;
	wColor.lpfnWndProc = colorWndProc;
	wColor.cbClsExtra = 0;
	wColor.cbWndExtra = 0;
	wColor.hInstance = hInstance;
	wColor.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wColor.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wColor.hCursor = LoadCursor(NULL, IDC_ARROW);
	wColor.hbrBackground = CreateSolidBrush(RGB(0, 0, 0)); //(HBRUSH)(COLOR_WINDOW + 1);
	wColor.lpszMenuName = NULL;
	wColor.lpszClassName = "Colors";

	if (!RegisterClassEx(&wColor))
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "Error", MB_OK | MB_ICONERROR);
	}

	hwndColor = CreateWindowEx( // color window
		WS_EX_LAYERED | WS_EX_TOPMOST,
		"Colors",
		"                                        Color Matrix",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		300,
		240,
		NULL,
		NULL,
		hInstance,
		NULL);
	std::string status = "MagFactor: ";
	status += to_string(MAGFACTOR);
	hWndStatus = CreateWindowEx(
		0,
		"Edit",
		status.c_str(),
		WS_VISIBLE | WS_CHILD | BS_TEXT,
		0,
		0,
		300,
		25,
		hwndColor,
		(HMENU)IDB_RADIO5,
		hInstance,
		NULL);
	if (!hwndColor)
	{
		MessageBox(NULL, "Window Creation Failed.", "Error", MB_OK | MB_ICONERROR);
		//return 0;
	}
	SetLayeredWindowAttributes(hwndColor, 255, 255, LWA_ALPHA);
	ShowWindow(hwndColor, SW_NORMAL);
	UpdateWindow(hwndColor);
	//ShowWindow(hwndHost, SW_HIDE);
	/*MSG    Msg;
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);

	}*/
}
LRESULT CALLBACK colorWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_BUTTON:
			switch (HIWORD(wParam))
			{
			case BN_CLICKED: // OK button clicked
							 //DeleteObject(s_hFont);
							 //DestroyWindow(hwndHost);
							 //RunMag(nCmd);
				DestroyWindow(hWnd);
				//PostQuitMessage(0);
				break;
			default:
				break;
			}
			break;
		}
	}
	case WM_MBUTTONDOWN:
		//(ShowMag == true) ? ShowMag = false : ShowMag = true;
	case WM_SYSKEYUP:

	case WM_KEYDOWN:
		//if (wParam == VK_ESCAPE)
		break;

	case WM_SYSCOMMAND:
		if (GET_SC_WPARAM(wParam) == SC_MAXIMIZE)
		{
			//GoFullScreen();
		}
		else
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		if (hwndMag != NULL)
		{
			GetClientRect(hWnd, &magWindowRect);
			// Resize the control to fill the window.
			SetWindowPos(hwndMag, NULL,
				magWindowRect.left, magWindowRect.top, magWindowRect.right, magWindowRect.bottom, 0);
		}
		break;

	default:return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
int RunMag(_In_ int       nCmdShow)
{
	if (FALSE == MagInitialize())
	{
		return 0;
	}
	if (FALSE == SetupMagnifier(hInst))
	{
		return 0;
	}

	UINT_PTR timerId;
	ShowWindow(hwndHost, nCmdShow);
	UpdateWindow(hwndHost);

	// Create a timer to update the control.
	timerId = SetTimer(hwndHost, 0, timerInterval, UpdateMagWindow);
	Mousehook = SetWindowsHookEx(WH_MOUSE_LL, HookCallback, NULL, 0);
	KeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
	
	// Main message loop.
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (runmag == false)break;
	}
	// Shut down.
	KillTimer(NULL, timerId);
	MagUninitialize();
	return 0;
	//return (int)msg.wParam;
}
LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_MBUTTONUP)
	{
		(ShowMag == true) ? ShowMag = false : ShowMag = true;
	}
	return CallNextHookEx(Mousehook, nCode, wParam, lParam);
}
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)  //KeyHookCallback
{
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
	{
		LPKBDLLHOOKSTRUCT temp = (LPKBDLLHOOKSTRUCT)lParam;
		if(temp->vkCode == VK_ESCAPE)
		(ShowMag == true) ? ShowMag = false : ShowMag = true;

		if (temp->vkCode ==  VK_SUBTRACT)
		{
			if (MAGFACTOR > 1.0f) { MAGFACTOR -= 1.0f; zoomChanged = true;	}
		}
		if (temp->vkCode ==  VK_ADD)
		{
			MAGFACTOR += 1.0f;
			zoomChanged = true;
			//(Invert == true) ? Invert = false : Invert = true;
			//colorChanged = true;
			//(showColorMatrix == true) ? showColorMatrix = false : showColorMatrix = true;
			//(showOptions == true) ? showOptions = false : showOptions = true;
			//ShowWindow(hwndColor, SW_NORMAL);
			//SetWindowPos(hwndColor, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW);
			//runmag = false;
			//DestroyWindow(hwndHost);
			//SettingsWindow(hInst);
			//RunMag(nCmd);
			//return (int)wParam;
			//ShowWindow(hwndColor, SW_SHOWNORMAL);
			//runmag = false;
			//ShowWindow(hwndHost, SW_HIDE);
			//ShowWindow(hwndMag, SW_HIDE);
		}
		if (temp->vkCode == VK_MULTIPLY)
		{
			if (Invert == false && Gray == false && HighContrast == false)
			{
				Invert = true;
			}
			else if (Invert == true)
			{
				Invert = false;
				Gray = true;
			}
			else if (Gray == true)
			{
				Gray = false;
				HighContrast = true;
			}
			else if (HighContrast == true)
			{
				HighContrast = false;
			}
			colorChanged = true;
		}
	}
	return CallNextHookEx(KeyHook, nCode, wParam, lParam);
}
///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// MAIN ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE /*hPrevInstance*/,
	_In_ LPSTR     /*lpCmdLine*/,
	_In_ int       nCmdShow)
{
	nCmd = nCmdShow;
	MagUninitialize();
	//SettingsWindow(hInstance); // launch settings

	//////////////////////////// Settings window ///////////////////////////////////////////////
	WNDCLASSEX    wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0)); //(HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "SettingsWin";

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "Error", MB_OK | MB_ICONERROR);
	}

	hWndSetup = CreateWindowEx( // Settings window
		WS_EX_OVERLAPPEDWINDOW,
		"SettingsWin",
		"                                        Select color mode & zoom level",
		WS_OVERLAPPEDWINDOW,
		200,
		100,
		600,
		800,
		NULL,
		NULL,
		hInstance,
		NULL);
	if (!hWndSetup)
	{
		MessageBox(NULL, "Window Creation Failed.", "Error", MB_OK | MB_ICONERROR);
		//return 0;
	}
	//SetLayeredWindowAttributes(hWndSetup, 255, 255, LWA_ALPHA);
	//ShowWindow(hWndSetup, SW_SHOW);
	ShowWindow(hWndSetup, SW_SHOWNORMAL);
	UpdateWindow(hWndSetup);

	// Main message loop.
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		//if (showColorMatrix == true) ShowColor(hInstance);
	}
	//ShowColor(hInstance);
	////////////////////////////// Mag Window /////////////////////////////////////////////
	while (Quit == false)
	{
		//RunMag(nCmdShow);
		Restart(nCmdShow);
	}
	

	return 0;
}
void Restart(_In_ int       nCmdShow)
{
	runmag = true;
	DestroyWindow(hwndMag);
	DestroyWindow(hwndHost);
	MagUninitialize();
	RunMag(nCmdShow);
	
}
// FUNCTION: HostWndProc()
//
// PURPOSE: Window procedure for the window that hosts the magnifier control.
//
LRESULT CALLBACK HostWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MBUTTONDOWN:
		(ShowMag == true) ? ShowMag = false : ShowMag = true;
	case WM_SYSKEYUP:

	case WM_KEYDOWN:
		//if (wParam == VK_ESCAPE)
		break;

	case WM_SYSCOMMAND:
		if (GET_SC_WPARAM(wParam) == SC_MAXIMIZE)
		{
			//GoFullScreen();
		}
		else
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		if (hwndMag != NULL)
		{
			GetClientRect(hWnd, &magWindowRect);
			// Resize the control to fill the window.
			SetWindowPos(hwndMag, NULL,
				magWindowRect.left, magWindowRect.top, magWindowRect.right, magWindowRect.bottom, 0);
		}
		break;

	default:return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//  FUNCTION: RegisterHostWindowClass()
//
//  PURPOSE: Registers the window class for the window that contains the magnification control.
//
ATOM RegisterHostWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = HostWndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(1 + COLOR_BTNFACE);
	wcex.lpszClassName = WindowClassName;

	return RegisterClassEx(&wcex);
}

// FUNCTION: SetupMagnifier
//
// PURPOSE: Creates the windows and initializes magnification.
//
BOOL SetupMagnifier(HINSTANCE hinst)
{
	POINT pt;
	GetCursorPos(&pt);
	hostWindowRect.top = pt.y - 100;
	hostWindowRect.bottom = pt.y + 100;
	hostWindowRect.left = pt.x - 100;
	hostWindowRect.right = pt.y + 100;
	// Set bounds of host window according to screen size.

	// Create the host window.
	RegisterHostWindowClass(hinst);
	hwndHost = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
		WindowClassName, 
		WindowTitle,
		RESTOREDWINDOWSTYLES,
		0, 0, hostWindowRect.right, hostWindowRect.bottom, NULL, NULL, hInst, NULL);
	if (!hwndHost)
	{
		return FALSE;
	}

	// Make the window opaque.
	SetLayeredWindowAttributes(hwndHost, 0, 255, LWA_ALPHA);
	SetWindowLongPtr(hwndHost, GWL_STYLE, 0); // make title bar go away
										   // Create a magnifier control that fills the client area.
	GetClientRect(hwndHost, &magWindowRect);

	hwndMag = CreateWindow(
		WC_MAGNIFIER,
		TEXT("MagnifierWindow"),
		WS_CHILD | WS_VISIBLE,
		magWindowRect.left, magWindowRect.top, magWindowRect.right, magWindowRect.bottom, hwndHost, NULL, hInst, NULL);
	if (!hwndMag)
	{
		return FALSE;
	}

	// Set the magnification factor.
	MAGTRANSFORM matrix;
	memset(&matrix, 0, sizeof(matrix));
	matrix.v[0][0] = MAGFACTOR;
	matrix.v[1][1] = MAGFACTOR;
	matrix.v[2][2] = 1.0f;

	BOOL ret = MagSetWindowTransform(hwndMag, &matrix);
	
	if (Invert == true)
	{
		if (ret)
		{// red, blue, green, alpha, and color translation
			MAGCOLOREFFECT magEffectInvert =
			{ { // MagEffectInvert
				{ -1.0f,  0.0f,  0.0f,  0.0f,  0.0f },
				{ 0.0f, -1.0f,  0.0f,  0.0f,  0.0f },
				{ 0.0f,  0.0f, -1.0f,  0.0f,  0.0f },
				{ 0.0f,  0.0f,  0.0f,  1.0f,  0.0f },
				{ 1.0f,  1.0f,  1.0f,  0.0f,  1.0f }
				} };

			//{ { // MagEffectInvert
			//	{ 1.0f,  0.0f, 0.0f, 0.0f, 0.0f },
			//	{ 0.0f,  0.6f,  0.6f,  0.0f,  0.0f },
			//	{ 0.1f,  0.1f,  -1.0f,  0.0f,  0.0f },
			//	{ 0.0f,  0.0f,  0.0f,  1.0f,  0.0f },
			//	{ 0.0f,  0.0f,  0.0f,  0.0f,  1.0f }
			//	} };

			ret = MagSetColorEffect(hwndMag, &magEffectInvert);
		}
	}
	else if (Gray == true)
	{
		if (ret)
		{
			MAGCOLOREFFECT magEffectGrayscale =
			{ { // MagEffectGrayscale
				{ 0.3f,  0.3f,  0.3f,  0.0f,  0.0f },
				{ 0.6f,  0.6f,  0.6f,  0.0f,  0.0f },
				{ 0.1f,  0.1f,  0.1f,  0.0f,  0.0f },
				{ 0.0f,  0.0f,  0.0f,  1.0f,  0.0f },
				{ 0.0f,  0.0f,  0.0f,  0.0f,  1.0f }
				} };

			ret = MagSetColorEffect(hwndMag, &magEffectGrayscale);
		}
	}
	else if (HighContrast == true)
	{
		if (ret)
		{
			MAGCOLOREFFECT highContrast =
			{ { // High contrast
				{ 2.0f, 0.0f, 0.0f, 0.0f, 0.0f },
				{ 0.0f, 2.0f,  0.0f,  0.0f,  0.0f },
				{ 0.0f,  0.0f, 2.0f,  0.0f,  0.0f },
				{ 0.0f,  0.0f,  0.0f,  1.0f,  0.0f },
				{ -0.5f,  -0.5f,  -0.5f,  0.0f,  1.0f }
				} };

			MagSetColorEffect(hwndMag, &highContrast);
		}
	}
	return ret;
}

// FUNCTION: UpdateMagWindow()
//
// PURPOSE: Sets the source rectangle and updates the window. Called by a timer.
//
void CALLBACK UpdateMagWindow(HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR /*idEvent*/, DWORD /*dwTime*/)
{
	POINT mousePoint;
	GetCursorPos(&mousePoint);
	POINT pt;
	GetCursorPos(&pt);
	switch (LensSize)
	{
	case 0:
		if (ShowMag == true)
		{
			SetLayeredWindowAttributes(hwndHost, 0, 255, LWA_ALPHA);
			SetWindowPos(hwndHost, HWND_TOP, pt.x - CURSORX_OFFSET, pt.y - CURSORY_OFFSET, 850, 400, SWP_SHOWWINDOW);
		}
		else
		{
			//ShowWindow(hwndHost, SW_HIDE);
			SetLayeredWindowAttributes(hwndHost, 0, 0, LWA_ALPHA);
			SetWindowPos(hwndHost, HWND_TOP, pt.x - CURSORX_OFFSET, pt.y - CURSORY_OFFSET, 850, 400, SWP_SHOWWINDOW);
		}
		break;
	case 1:
		if (ShowMag == true)
		{
			SetLayeredWindowAttributes(hwndHost, 0, 255, LWA_ALPHA);
			SetWindowPos(hwndHost, HWND_TOP, pt.x - CURSORX_OFFSET_M, pt.y - CURSORY_OFFSET_M, 1024, 600, SWP_SHOWWINDOW);
		}
		else
		{
			SetLayeredWindowAttributes(hwndHost, 0, 1, LWA_ALPHA);
			SetWindowPos(hwndHost, HWND_TOP, pt.x - CURSORX_OFFSET_M, pt.y - CURSORY_OFFSET_M, 1024, 600, SWP_SHOWWINDOW);
		}
		break;
	case 2:
		if (ShowMag == true)
		{
			SetLayeredWindowAttributes(hwndHost, 0, 255, LWA_ALPHA);
			SetWindowPos(hwndHost, HWND_TOP, pt.x - CURSORX_OFFSET_L, pt.y - CURSORY_OFFSET_L, 1200, 800, SWP_SHOWWINDOW);
		}
		else
		{
			SetLayeredWindowAttributes(hwndHost, 0, 1, LWA_ALPHA);
			SetWindowPos(hwndHost, HWND_TOP, pt.x - CURSORX_OFFSET_L, pt.y - CURSORY_OFFSET_L, 1200, 800, SWP_SHOWWINDOW);
		}
		break;
	default:
		break;
	}
	if (zoomChanged == true)
	{
		// Set the magnification factor.
		MAGTRANSFORM matrix;
		memset(&matrix, 0, sizeof(matrix));
		matrix.v[0][0] = MAGFACTOR;
		matrix.v[1][1] = MAGFACTOR;
		matrix.v[2][2] = 1.0f;

		MagSetWindowTransform(hwndMag, &matrix);
		zoomChanged = false;
	}

	if (colorChanged == true)
	{
		if (Invert == true)
		{
			if (1)
			{// red, blue, green, alpha, and color translation
				MAGCOLOREFFECT magEffectInvert =
				//{ { // MagEffectInvert
				//	{ -1.0f,  0.0f,  0.0f,  0.0f,  0.0f },
				//	{ 0.0f, -1.0f,  0.0f,  0.0f,  0.0f },
				//	{ 0.0f,  0.0f, -1.0f,  0.0f,  0.0f },
				//	{ 0.0f,  0.0f,  0.0f,  1.0f,  0.0f },
				//	{ 1.0f,  1.0f,  1.0f,  0.0f,  1.0f }
				//	} };

				//{ { // High contrast
				//	{ 2.0f,  0.0f,  0.0f,  0.0f,  0.0f },
				//	{ 0.0f, 2.0f,  0.0f,  0.0f,  0.0f },
				//	{ 0.0f,  0.0f, 2.0f,  0.0f,  0.0f },
				//	{ 0.0f,  0.0f,  0.0f,  1.0f,  0.0f },
				//	{ -0.5f,  -0.5f,  -0.5f,  0.0f,  1.0f }
				//	} };
			
				//{ { // High Contrast * GrayScale
				//	{ 0.6f, 0.6f, 0.6f, 0.0f, 0.0f },
				//	{ 1.2f,  1.2f,  1.2f,  0.0f,  0.0f },
				//	{ 0.2f,  0.2f,  0.2f,  0.0f,  0.0f },
				//	{ 0.0f,  0.0f,  0.0f,  1.0f,  0.0f },
				//	{ -0.5f,  -0.5f,  -0.5f,  0.0f,  1.0f }
				//	} };

				//{ { // Invert * High contrast
				//	{ -2.0f,  0.0f,  0.0f,  0.0f,  0.0f },
				//	{ 0.0f, -2.0f,  0.0f,  0.0f,  0.0f },
				//	{ 0.0f,  0.0f, -2.0f,  0.0f,  0.0f },
				//	{ 0.0f,  0.0f,  0.0f,  1.0f,  0.0f },
				//	{ 1.5f,  1.5f,  1.5f,  0.0f,  1.0f }
				//	} };
				
				{ { // Remove blue
					{ 1.0f,  0.0f,  0.0f,  0.0f,  0.0f },
					{ 0.0f,  1.0f,  0.0f,  0.0f,  0.0f },
					{ 0.0f,  0.0f,  1.0f,  0.0f,  0.0f },
					{ 0.0f,  0.0f,  -1.0f,  1.0f,  0.0f },
					{ 0.0f,  0.0f,  0.0f,  0.0f,  1.0f }
					} };

				/*values = "1   0   0   0   0
							0   1   0   0   0
							0   0   1   0   0
							0   0 - 2   1   0 "/*/

				MagSetColorEffect(hwndMag, &magEffectInvert);
			}
		}
		else if (Gray == true)
		{
			if (1)
			{
				MAGCOLOREFFECT magEffectGrayscale =
				{ { // MagEffectGrayscale
					{ 0.3f,  0.3f,  0.3f,  0.0f,  0.0f },
					{ 0.6f,  0.6f,  0.6f,  0.0f,  0.0f },
					{ 0.1f,  0.1f,  0.1f,  0.0f,  0.0f },
					{ 0.0f,  0.0f,  0.0f,  1.0f,  0.0f },
					{ 0.0f,  0.0f,  0.0f,  0.0f,  1.0f }
					} };
				
				MagSetColorEffect(hwndMag, &magEffectGrayscale);
			}

		}
		else if (HighContrast == true)
		{
			if (1)
			{
				MAGCOLOREFFECT highContrast =
				{ { // High contrast
					{ 2.0f, 0.0f, 0.0f, 0.0f, 0.0f },
					{ 0.0f, 2.0f,  0.0f,  0.0f,  0.0f },
					{ 0.0f,  0.0f, 2.0f,  0.0f,  0.0f },
					{ 0.0f,  0.0f,  0.0f,  1.0f,  0.0f },
					{ -0.5f,  -0.5f,  -0.5f,  0.0f,  1.0f }
					} };

				MagSetColorEffect(hwndMag, &highContrast);
			}
		}
		else
		{
			MagSetColorEffect(hwndMag, NULL);
		}
		colorChanged = false;
	}
	
	int width = (int)((magWindowRect.right - magWindowRect.left) / MAGFACTOR);
	int height = (int)((magWindowRect.bottom - magWindowRect.top) / MAGFACTOR);
	RECT sourceRect;
	sourceRect.left = mousePoint.x - width / 2;
	sourceRect.top = mousePoint.y - height / 2;

	// Don't scroll outside desktop area.
	switch (LensSize)
	{
	case 0:
		if (sourceRect.left < 0 - CURSORX_OFFSET)
			sourceRect.left = 0;
		if (sourceRect.left > GetSystemMetrics(SM_CXVIRTUALSCREEN) - width + CURSORX_OFFSET)
			sourceRect.left = GetSystemMetrics(SM_CXVIRTUALSCREEN) - width;
		sourceRect.right = sourceRect.left + width;
		if (sourceRect.top < 0 - CURSORY_OFFSET)
			sourceRect.top = 0;
		if (sourceRect.top > GetSystemMetrics(SM_CYVIRTUALSCREEN) - height + CURSORY_OFFSET)
			sourceRect.top = GetSystemMetrics(SM_CYVIRTUALSCREEN) - height;
		sourceRect.bottom = sourceRect.top + height;
		break;
	case 1:
		if (sourceRect.left < 0 - CURSORX_OFFSET_M)
			sourceRect.left = 0;
		if (sourceRect.left > GetSystemMetrics(SM_CXVIRTUALSCREEN) - width + CURSORX_OFFSET_M)
			sourceRect.left = GetSystemMetrics(SM_CXVIRTUALSCREEN) - width;
		sourceRect.right = sourceRect.left + width;
		if (sourceRect.top < 0 - CURSORY_OFFSET_M)
			sourceRect.top = 0;
		if (sourceRect.top > GetSystemMetrics(SM_CYVIRTUALSCREEN) - height + CURSORY_OFFSET_M)
			sourceRect.top = GetSystemMetrics(SM_CYVIRTUALSCREEN) - height;
		sourceRect.bottom = sourceRect.top + height;
		break;
	case 2:
		if (sourceRect.left < 0 - CURSORX_OFFSET_L)
			sourceRect.left = 0;
		if (sourceRect.left > GetSystemMetrics(SM_CXVIRTUALSCREEN) - width + CURSORX_OFFSET_L)
			sourceRect.left = GetSystemMetrics(SM_CXVIRTUALSCREEN) - width;
		sourceRect.right = sourceRect.left + width;
		if (sourceRect.top < 0 - CURSORY_OFFSET_L)
			sourceRect.top = 0;
		if (sourceRect.top > GetSystemMetrics(SM_CYVIRTUALSCREEN) - height + CURSORY_OFFSET_L)
			sourceRect.top = GetSystemMetrics(SM_CYVIRTUALSCREEN) - height;
		sourceRect.bottom = sourceRect.top + height;
		break;
	default:
		break;
	}

	if (ShowMag == true)
	{
		// Set the source rectangle for the magnifier control.
		MagSetWindowSource(hwndMag, sourceRect);

		if (showOptions == false)
		{
			// Reclaim topmost status, to prevent unmagnified menus from remaining in view. 
			SetWindowPos(hwndHost, HWND_TOPMOST, 0, 0, 0, 0,
				SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
			// Force redraw.
			InvalidateRect(hwndMag, NULL, TRUE);
		}
		
		
	}
}