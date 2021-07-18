#pragma once
#ifndef _WINDOWS_INFO_H
#define _WINDOWS_INFO_H
#include "Common/DirectxCore.h"

class WindowInfo
{
private:
	HWND hwnd;
	HINSTANCE hinstance;
	int screenWidth;
	int screenHeight;

private:
	static shared_ptr<WindowInfo> single;

public:
	WindowInfo();
	~WindowInfo();

public:
	HWND GetHwnd();
	HINSTANCE GetHinstance();
	int GetScreenWidth();
	int GetScreenHeight();
	void Init(HWND inHwnd, HINSTANCE inHinstance, int inScreenWidth, int inScreenHeight);

public:
	static shared_ptr<WindowInfo> Get();
};

#define GWindowInfo WindowInfo::Get()
#define GHinstance GWindowInfo->GetHinstance()
#define GHwnd GWindowInfo->GetHwnd()
#define GScreenWidth GWindowInfo->GetScreenWidth()
#define GScreenHeight GWindowInfo->GetScreenHeight()
#endif // !_WINDOWS_INFO_H
