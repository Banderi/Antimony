#pragma once

#include <stdio.h>
#include <iostream>

#include "Window.h"

///

void InitializeDebugConsole();
void ShutdownDebugConsole();

HRESULT WriteToConsole(std::wstring string);
void LogError(HRESULT hr);