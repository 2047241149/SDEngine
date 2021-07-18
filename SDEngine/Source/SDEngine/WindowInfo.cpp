#include "WindowInfo.h"

WindowInfo::WindowInfo()
{
}

WindowInfo::~WindowInfo()
{
}

shared_ptr<WindowInfo> WindowInfo::Get()
{
	if (single == nullptr)
	{
		single = shared_ptr<WindowInfo>(new WindowInfo());
	}

	return single;
}

void WindowInfo::Init(HWND inHwnd, HINSTANCE inHinstance, int inScreenWidth, int inScreenHeight)
{
	hwnd = inHwnd;
	hinstance = inHinstance;
	screenWidth = inScreenWidth;
	screenHeight = inScreenHeight;
}

HINSTANCE WindowInfo::GetHinstance()
{
	return hinstance;
}

HWND WindowInfo::GetHwnd()
{
	return hwnd;
}

int WindowInfo::GetScreenWidth()
{
	return screenWidth;
}

int WindowInfo::GetScreenHeight()
{
	return screenHeight;
}

shared_ptr<WindowInfo> WindowInfo::single = nullptr;