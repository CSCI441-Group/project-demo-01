#include <windows.h>
#include <string>
#include <iostream>

#define FILE_MENU_LOGIN 1
#define FILE_MENU_LOGOUT 2
#define FILE_MENU_EXIT 3

LRESULT CALLBACK testWindow(HWND, UINT, WPARAM, LPARAM);

void AddMenu(HWND);
void StartScreen(HWND);

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

	CreateWindowW(L"WindowClass", L"Login", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 500, NULL, NULL, NULL, NULL);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
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
	CreateWindowW(L"static", L"\t\tPOS\n\n\tChoose a menu function", WS_VISIBLE | WS_CHILD, 100, 100, 300, 50, hWnd, NULL, NULL, NULL);
}