#include <windows.h>
#include <string>
#include <iostream>

#define FILE_MENU_LOGIN 1
#define FILE_MENU_LOGOUT 2
#define FILE_MENU_EXIT 3

LRESULT CALLBACK testWindow(HWND, UINT, WPARAM, LPARAM);

void AddMenu(HWND);
void StartScreen(HWND);
void LoginScreen(HWND);
void ClearScreen(HWND);

HMENU hMenu;

int main(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
	WNDCLASSW test = { 0 };

	test.hbrBackground = (HBRUSH)COLOR_WINDOW;
	test.hCursor = LoadCursor(NULL, IDC_ARROW);
	test.hInstance = hInst;
	test.lpszClassName = L"WindowClass";
	test.lpfnWndProc = testWindow;

	if (!RegisterClassW(&test))
		return -1;

	CreateWindowW(L"WindowClass", L"Diner POS", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 500, NULL, NULL, NULL, NULL);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//this is just for easy copy paste for testing functionality
	//MessageBoxW(NULL, L"hello", L"Testing", MB_OK);
	return 0;
}

LRESULT CALLBACK testWindow(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (wp)
		{
		case FILE_MENU_EXIT:
			DestroyWindow(hWnd);
			break;
		case FILE_MENU_LOGIN:
			LoginScreen(hWnd);
			break;
		case FILE_MENU_LOGOUT:
			break;
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

void AddMenu(HWND hWnd)
{
	hMenu = CreateMenu();
	HMENU hOperations = CreateMenu();

	AppendMenu(hOperations, MF_STRING, FILE_MENU_LOGIN, L"Login");
	AppendMenu(hOperations, MF_STRING, FILE_MENU_LOGOUT, L"Logout");
	AppendMenu(hOperations, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hOperations, MF_STRING, FILE_MENU_EXIT, L"Exit");

	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hOperations, L"Operations");
	SetMenu(hWnd, hMenu);
}

void StartScreen(HWND hWnd)
{
	CreateWindowW(L"STATIC", L"\nDiner POS", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 200, 70, 100, 50, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"STATIC", L"Choose a menu function", WS_VISIBLE | WS_CHILD | SS_CENTER, 100, 150, 300, 60, hWnd, NULL, NULL, NULL);
}

void LoginScreen(HWND hWnd)
{
	CreateWindowW(L"STATIC", L"Enter login credentials", WS_VISIBLE | WS_CHILD | SS_CENTER, 150, 150, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"EDIT", L". . .", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 175, 300, 20, hWnd, NULL, NULL, NULL);
	//perhaps use ES_PASSWORD for password child windows
	//find out how to grab text input in box and read into main
	HWND loginButton = CreateWindowW(L"BUTTON", L"Login", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 200, 210, 100, 40, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	//find out how to recognize button input
}

void ClearScreen(HWND hWnd)
{
	//run if statements to find any open child windows
	//find way to grab identifier and use CloseWindow function on child windows ie. CloseWindow(WS_CHILD identifier)
}