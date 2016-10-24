#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>

extern HWND hWnd;
extern int wWidth, wHeight, wX, wY;
extern RECT wScreen;
extern bool wFullscreen, wBorderless, wVSync;
extern float wAspectRatio;

extern unsigned int numerator, denominator;
extern int videoCardMemory;
extern char videoCardDescription[128];

void CreateMainWindow(HINSTANCE hInstance);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif