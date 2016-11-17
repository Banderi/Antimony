#pragma once

#include <stdio.h>
#include <iostream>

void InitializeDebugConsole();
void ShutdownDebugConsole();

HRESULT WriteToConsole(std::wstring string);
void LogError(HRESULT hr);