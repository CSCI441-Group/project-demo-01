#pragma once
#include <windows.h>
#include <string>
#include <iostream>

#define FILE_MENU_LOGIN 1
#define FILE_MENU_LOGOUT 2
#define FILE_MENU_EXIT 3
#define LOGIN_BUTTON 4
#define CANCEL_LOGIN_BUTTON 5
#define	IDC_GROUP1 6
#define	IDC_GROUP2 7
#define	IDC_GROUP3 8
#define IDC_RESET 9
#define IDC_SUBMIT 10

LRESULT CALLBACK MainWindow(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DestoryChildCallback(HWND, LPARAM);

void AddMenu(HWND);
void StartScreen(HWND);
void LoginScreen(HWND);
void ClearScreen(HWND);
void CreateGroups(HWND);
void Createopbutton(HWND);

HMENU hMenu;
HWND cancelButton;

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

char psswrd[256] = "",
user[256] = "",
first[256] = "",
last[256] = "";

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
			CreateGroups(hWnd);
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

void CreateTableMenu(HWND hWnd)
{
	CreateGroups(hWnd);
}

void CreateGroups(HWND hWnd)
{
	//CREATES THE OUTLINE FOR THE FIRST GROUP
	CreateWindowW(L"Button", L"TABLE DETAILS", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 30, 50, 400, 250, hWnd,
		(HMENU)IDC_GROUP1, NULL, NULL);

	//CREATES THE OUTLINE FOR THE SECOND GROUP
	CreateWindowW(L"Button", L"ORDER DETAILS", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 500, 50, 400,
		250, hWnd, (HMENU)IDC_GROUP2, NULL, NULL);

	//CREATES THE OUTLINE FOR THE THIRD GROUP
	CreateWindowW(L"Button", L"TAB", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 30, 320, 670,
		120, hWnd, (HMENU)IDC_GROUP3, NULL, NULL);
}

