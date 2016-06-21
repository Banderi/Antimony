#include "Window.h"
#include "DebugWin.h"

HWND hWnd;
int wWidth, wHeight, wX, wY;
RECT wScreen;
bool wFullscreen, wBorderless, wVSync;
float wAspectRatio;

unsigned int numerator, denominator;
int videoCardMemory;
char videoCardDescription[128];

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

	hWnd = CreateWindowExW(0,
		L"WindowClass",
		L"Project X",
		DWORD(wBorderless * WS_POPUP) | WS_VISIBLE | WS_SYSMENU,
		wX, wY,
		wWidth, wHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	WriteToConsole(L" done\n");
}