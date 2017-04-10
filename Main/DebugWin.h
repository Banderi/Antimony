#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>

#include "Window.h"

///

extern std::wofstream logfile;

///

void InitializeDebugConsole();
void ShutdownDebugConsole();

HRESULT WriteToConsole(std::wstring string, bool t = true, bool logf = true);
void LogError(HRESULT hr);

std::wstring ctowstring(char* c);