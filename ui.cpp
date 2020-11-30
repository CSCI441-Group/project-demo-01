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

void AddMenu(HWND);
void StartScreen(HWND);
void LoginScreen(HWND);
void ClearScreen(HWND);
void CreateGroups(HWND);
void Createopbutton(HWND);

HMENU hMenu;
HWND cancelButton;

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
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_CLIPSIBLINGS, 
		0, 
		0, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		NULL, 
		NULL, 
		NULL, 
		NULL
	);

	MSG msg = { 0 };
	ShowWindow(hWnd, SW_MAXIMIZE);

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
			LoginScreen(hWnd);
			break;
		case FILE_MENU_LOGOUT:
			break;
		case LOGIN_BUTTON:
			//put login functionality here
			CreateGroups(hWnd);
			break;
		case CANCEL_LOGIN_BUTTON:
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
	RECT rc;
	GetWindowRect(hWnd, &rc);
	int xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;

	CreateWindowW(L"STATIC", L"\nDiner POS", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, xPos, yPos, 100, 50, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"STATIC", L"Choose a menu function", WS_VISIBLE | WS_CHILD | SS_CENTER, xPos, yPos, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, NULL, NULL);
}

void LoginScreen(HWND hWnd)
{

	CreateWindowW(L"STATIC", L"Enter login credentials", WS_VISIBLE | WS_CHILD | SS_CENTER, 150, 150, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"EDIT", L". . .", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 175, 300, 20, hWnd, NULL, NULL, NULL);
	//perhaps use ES_PASSWORD for password child windows
	//find out how to grab text input in box and read into main
	HWND loginButton = CreateWindowW(L"BUTTON", L"Login", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 200, 210, 100, 40, hWnd, (HMENU)LOGIN_BUTTON, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	HWND cancelButton = CreateWindowW(L"BUTTON", L"Cancel", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 200, 260, 100, 40, hWnd, (HMENU)CANCEL_LOGIN_BUTTON, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
}

void CreateTableMenu(HWND phWnd)
{
	CreateGroups(phWnd);
	Createopbutton(phWnd);
}

void CreateGroups(HWND phWnd)
{
	//CREATES THE OUTLINE FOR THE FIRST GROUP
	CreateWindowW(L"Button", L"TABLE DETAILS", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 30, 50, 400, 250, phWnd,
		(HMENU)IDC_GROUP1, NULL, NULL);

	//CREATES THE OUTLINE FOR THE SECOND GROUP
	CreateWindowW(L"Button", L"ORDER DETAILS", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 500, 50, 400,
		250, phWnd, (HMENU)IDC_GROUP2, NULL, NULL);

	//CREATES THE OUTLINE FOR THE THIRD GROUP
	CreateWindowW(L"Button", L"TAB", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 30, 320, 670,
		120, phWnd, (HMENU)IDC_GROUP3, NULL, NULL);

}

void Createopbutton(HWND phWnd)
{
	RECT rc;
	GetWindowRect(phWnd, &rc);
	int xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;

	//CREATES THE SUBMIT BUTTON
	CreateWindowW(L"Button", L"Submit", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP, xPos, yPos, 100, 30,
		phWnd, (HMENU)IDC_SUBMIT, NULL, NULL);

	//CREATES THE RESET BUTTON
	CreateWindowW(L"Button", L"Reset", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP, xPos, yPos, 100, 30,
		phWnd, (HMENU)IDC_RESET, NULL, NULL);

}

void ClearScreen(HWND hWnd)
{
	//run if statements to find any open child windows
	//find way to grab identifier and use CloseWindow function on child windows ie. CloseWindow(WS_CHILD identifier)
}

