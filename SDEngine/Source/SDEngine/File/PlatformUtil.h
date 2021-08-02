#pragma once
#include <string>
#include <windows.h>
using std::string;


class FileDialog
{
public:
	static string OpenFile(LPCWSTR fliterName);
	static string SaveFile(LPCWSTR fliterName);
};