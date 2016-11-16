#include "Window.h"
#include "DebugWin.h"

void CreateMainWindow(HINSTANCE hInstance)
{
	WriteToConsole(L"Creating main window handle...");

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

	windowMain.hWnd = CreateWindowExW(0,
		L"WindowClass",
		L"Project X",
		DWORD(windowMain.borderless * WS_POPUP) | WS_VISIBLE | WS_SYSMENU,
		windowMain.X, windowMain.Y,
		windowMain.width, windowMain.height,
		NULL,
		NULL,
		hInstance,
		NULL);

	WriteToConsole(L" done\n");
}