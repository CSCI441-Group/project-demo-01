
#include <iostream>
#include <windows.h>

#include "application.h"

LRESULT CALLBACK MainWindow(HWND, UINT, WPARAM, LPARAM);

int main(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
	WNDCLASSW kOrders = { 0 };
	HWND hWnd;

	kOrders.hbrBackground = (HBRUSH)COLOR_WINDOW;
	kOrders.hCursor = LoadCursor(NULL, IDC_ARROW);
	kOrders.hInstance = hInst;
	kOrders.lpszClassName = L"WindowClass";
	kOrders.lpfnWndProc = MainWindow;

	if (!RegisterClassW(&kOrders))
		return -1;

	hWnd = CreateWindowW(L"WindowClass", L"Diner POS", WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, 1200, 800, NULL, NULL, hInst, NULL);
	MSG msg = { 0 };

	try
	{
		Application application{};

		while (GetMessage((&msg), NULL, NULL, NULL))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	catch (...)
	{
		std::cout << "An error occurred!" << std::endl;
	}

	return 0;
}

LRESULT CALLBACK MainWindow(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
		break;
	}
}