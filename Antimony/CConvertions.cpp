#include <locale>
#include <codecvt>

#include "CConvertions.h"

///

const char* cc_cs(const wchar_t* ws)			// wchar_T*	-->	char*
{
	return cc_str(cc_wstr(ws)).c_str();
}
const char* cc_cs(std::string str)				// string	-->	char*
{
	return str.c_str();
}
const char* cc_cs(std::wstring wstr)			// wstring	-->	char*
{
	return cc_str(wstr).c_str();
}

const wchar_t* cc_wcs(const char* cs)			// char*	-->	wchar_t*
{
	return cc_wstr(cs).c_str();
}
const wchar_t* cc_wcs(std::string str)			// string	-->	wchar_t*
{
	return cc_wstr(str).c_str();
}
const wchar_t* cc_wcs(std::wstring wstr)		// wstring	-->	wchar_t*
{
	return wstr.c_str();
}

std::string cc_str(const char* cs)				// char*	-->	string
{
	std::string str(cs);
	return str;
}
std::string cc_str(const wchar_t* ws)			// wchar_T*	-->	string
{
	return cc_str(cc_cs(ws));
}
std::string cc_str(std::wstring wstr)			// wstring	-->	string
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	const std::string str = converter.to_bytes(wstr);
	return str;
}

std::wstring cc_wstr(const char* cs)			// char*	-->	wstring
{
	return cc_wstr(cc_str(cs));
}
std::wstring cc_wstr(const wchar_t* ws)			// wchar_T*	-->	wstring
{
	std::wstring wstr(ws);
	return wstr;
}
std::wstring cc_wstr(std::string str)			// string	-->	wstring
{
	std::wstring wstr;
	wstr.assign(str.begin(), str.end());
	return wstr;
}

