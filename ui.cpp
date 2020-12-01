#pragma once
#include <windows.h>
#include <commctrl.h>
#include <strsafe.h>
#include <string>
#include <iostream>

#define FILE_MENU_LOGIN 1
#define FILE_MENU_LOGOUT 2
#define FILE_MENU_EXIT 3
#define LOGIN_BUTTON 4
#define CANCEL_LOGIN_BUTTON 5
#define	IDC_LIST 6
#define	IDC_STATIC 7
#define	IDC_GROUP3 8
#define IDC_RESET 9
#define IDC_SUBMIT 10

LRESULT CALLBACK MainWindow(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DestoryChildCallback(HWND, LPARAM);

void AddMenu(HWND);
void StartScreen(HWND);
void LoginScreen(HWND);
void ClearScreen(HWND);
void CreateGroups(HWND, WPARAM);

static HWND hwndList, hwndStatic;
HMENU hMenu;

int  windowWidth = 1200,
windowHeight = 800,
buttonWidth = windowWidth / 5,
buttonHeight = windowHeight / 20,
bottomButton = windowHeight - 200,
centerX = (windowWidth / 2 - buttonWidth / 2),
centerY = (windowHeight / 2 - buttonHeight / 2) - 100,
headingWidth = buttonWidth * 2,
headingHeight = buttonHeight * 2,
headingCenterX = (windowWidth / 2 - headingWidth / 2);

wchar_t buf[128];

//char psswrd[256] = "",
//user[256] = "",
//first[256] = "",
//last[256] = "";

// For testing purposes
typedef struct {
	wchar_t first[20];
	wchar_t last[20];
	wchar_t type[20];
} Employees;

Employees employees[] = {
	{L"Ted", L"Jones", L"waitstaff"},
	{L"Thomas",L"Train", L"manager"},
	{L"George",L"Harrison", L"busboy"},
	{L"Michael",L"Jordan", L"kitchen"},
	{L"Janis",L"Joplin", L"host"},
};

int main(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
	WNDCLASSW test = { 0 };
	HWND hWnd;

	test.hbrBackground = (HBRUSH)COLOR_WINDOW;
	test.hCursor = LoadCursor(NULL, IDC_ARROW);
	test.hInstance = hInst;
	test.lpszClassName = L"WindowClass";
	test.lpfnWndProc = MainWindow;

	if (!RegisterClassW(&test))
		return -1;

	hWnd = CreateWindowW(L"WindowClass", 
		L"Diner POS", 
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS, 
		0, 
		0, 
		1200, 
		800, 
		NULL, 
		NULL, 
		NULL, 
		NULL
	);

	MSG msg = { 0 };

	// Maybe going full screen isnt a great idea because its harder to grab the center of the screen
	//ShowWindow(hWnd, SW_MAXIMIZE);

	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//this is just for easy copy paste for testing functionality
	//MessageBoxW(NULL, L"hello", L"Testing", MB_OK);
	return 0;
}

LRESULT CALLBACK MainWindow(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_COMMAND:
		if (LOWORD(wp) == IDC_LIST) {
			if (HIWORD(wp) == LBN_SELCHANGE) {
				int sel = (int)SendMessageW(hwndList, LB_GETCURSEL, 0, 0);
				StringCbPrintfW(buf, ARRAYSIZE(buf), L"Last Name: %ls \nType: %ls",
					employees[sel].last, employees[sel].type);
				SetWindowTextW(hwndStatic, buf);
			}
		}
		switch (LOWORD(wp))
		{
		case FILE_MENU_EXIT:
			if ((MessageBox(hWnd, L"Are you sure?", L"Message", MB_OKCANCEL)) == IDOK) {
				DestroyWindow(hWnd);
			}
			break;
		case FILE_MENU_LOGIN:
			// declare this statement to destroy and child windows before loading the next window
			EnumChildWindows(hWnd, DestoryChildCallback, NULL);
			LoginScreen(hWnd);
			break;
		case FILE_MENU_LOGOUT:
			EnumChildWindows(hWnd, DestoryChildCallback, NULL);
			break;
		case LOGIN_BUTTON:
			EnumChildWindows(hWnd, DestoryChildCallback, NULL);
			//put login functionality here
			CreateGroups(hWnd, wp);
			break;
		case CANCEL_LOGIN_BUTTON:
			EnumChildWindows(hWnd, DestoryChildCallback, NULL);
			StartScreen(hWnd);
			UpdateWindow(hWnd);
		}
		break;
	case WM_CREATE:
		AddMenu(hWnd);
		StartScreen(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
		break;
	}
}

// Destroys child window
BOOL CALLBACK DestoryChildCallback(HWND hwnd, LPARAM lParam)
{
	if (hwnd != NULL) {
		DestroyWindow(hwnd);
	}
	return TRUE;
}

void AddMenu(HWND hWnd)
{
	hMenu = CreateMenu();
	HMENU hOperations = CreateMenu();

	AppendMenu(hOperations, MF_STRING, FILE_MENU_LOGIN, L"Login");
	AppendMenu(hOperations, MF_STRING, FILE_MENU_LOGOUT, L"Logout");
	AppendMenu(hOperations, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hOperations, MF_STRING, FILE_MENU_EXIT, L"Exit");

	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hOperations, L"Operations");
	AppendMenuW(hMenu,MF_STRING,NULL,L"Help");
	SetMenu(hWnd, hMenu);
}

void StartScreen(HWND hWnd)
{
	CreateWindowW(L"STATIC", L"\nDiner POS", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, centerX, centerY + (buttonHeight + 2) * 1, buttonWidth, buttonHeight + 10, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"STATIC", L"Choose a menu function", WS_VISIBLE | WS_CHILD | SS_CENTER, centerX, centerY + (buttonHeight + 2) * 2, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, NULL, NULL);
}

void LoginScreen(HWND hWnd)
{
	CreateWindowW(L"STATIC", L"Enter login credentials", WS_VISIBLE | WS_CHILD | SS_CENTER, centerX, centerY + (buttonHeight + 2)*-1, buttonWidth, buttonHeight, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"EDIT", L"username...", WS_VISIBLE | WS_CHILD | WS_BORDER, centerX, centerY + (buttonHeight + 2) * 1, buttonWidth, buttonHeight, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"EDIT", L"password...", WS_VISIBLE | WS_CHILD | WS_BORDER, centerX, centerY + (buttonHeight + 2) * 2, buttonWidth, buttonHeight, hWnd, NULL, NULL, NULL);
	//perhaps use ES_PASSWORD for password child windows
	//find out how to grab text input in box and read into main
	HWND loginButton = CreateWindowW(L"BUTTON", L"Login", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, centerX, centerY + (buttonHeight + 2) * 3, buttonWidth, buttonHeight, hWnd, (HMENU)LOGIN_BUTTON, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	HWND cancelButton = CreateWindowW(L"BUTTON", L"Cancel", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, centerX, centerY + (buttonHeight + 2) * 4, buttonWidth, buttonHeight, hWnd, (HMENU)CANCEL_LOGIN_BUTTON, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
}

void CreateGroups(HWND hWnd, WPARAM wp)
{
	hwndList = CreateWindowW(WC_LISTBOXW, NULL, WS_CHILD | WS_VISIBLE | LBS_NOTIFY, 10, 10, 150, 120, hWnd, (HMENU)IDC_LIST, NULL, NULL);
	hwndStatic = CreateWindowW(WC_STATICW, NULL, WS_CHILD | WS_VISIBLE, 200, 10, 120, 45, hWnd, (HMENU)IDC_STATIC, NULL, NULL);

	for (int i = 0; i < ARRAYSIZE(employees); i++) {
		SendMessageW(hwndList, LB_ADDSTRING, 0, (LPARAM)employees[i].first);
	}
}

