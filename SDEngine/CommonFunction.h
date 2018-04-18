#pragma once
#ifndef _COMMON_FUNCTION_H
#define _COMMON_FUNCTION_H
#include<iostream>
#include<string>
using namespace std;

static wstring Str2Wstr(string str)
{
	if (str.length() == 0)
		return L"";

	std::wstring wstr;
	wstr.assign(str.begin(), str.end());
	return wstr;
}

#endif // !_COMMON_FUNCTION_H
