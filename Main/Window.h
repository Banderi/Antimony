#ifndef WINDOW_H
#define WINDOW_H

#include "Param.h"

void CreateMainWindow(HINSTANCE hInstance);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif