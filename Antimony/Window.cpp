#include "Antimony.h"

///

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void Antimony::createMainWindow(HINSTANCE hInstance)
{
	writeToConsole(L"Creating main window handle...");

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wc.lpszClassName = L"WindowClass";
	RegisterClassExW(&wc);

	window_main.hWnd = CreateWindowExW(0,
		L"WindowClass",
		L"Project X",
		DWORD(window_main.borderless * WS_POPUP) | WS_VISIBLE | WS_SYSMENU,
		window_main.x, window_main.y,
		window_main.width, window_main.height,
		NULL,
		NULL,
		hInstance,
		NULL);

	writeToConsole(L" done\n", false);
}