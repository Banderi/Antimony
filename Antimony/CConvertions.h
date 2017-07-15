#pragma once

#include <string>

///

const char* cc_cs(const wchar_t* ws);
const char* cc_cs(std::string str);
const char* cc_cs(std::wstring wstr);

const wchar_t* cc_wcs(const char* cs);
const wchar_t* cc_wcs(std::string str);
const wchar_t* cc_wcs(std::wstring wstr);

std::string cc_str(const char* cs);
std::string cc_str(const wchar_t* ws);
std::string cc_str(std::wstring wstr);

std::wstring cc_wstr(const char* cs);
std::wstring cc_wstr(const wchar_t* ws);
std::wstring cc_wstr(std::string str);

