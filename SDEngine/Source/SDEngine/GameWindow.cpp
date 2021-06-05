﻿#include "GameWindow.h"
#include "Common/CommonFunction.h"
#include "Event/Event.h"

static GameWindow* gameWindow = nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return gameWindow->MessageHandler(hwnd, message, wParam, lParam);
}

GameWindow::GameWindow():
		bStartEvent(false)
{
}

GameWindow::~GameWindow()
{
	ShutDown();
}

void GameWindow::Init(const WindowProp& props)
{
	data.width = props.width;
	data.height = props.height;
	data.title = props.tile;
	data.bVSync = false;
	data.fullScreen = false;
	gameWindow = this;
	InitWindow();
}

void GameWindow::InitWindow()
{
	WNDCLASSEX wc;
	DEVMODE dmScrrenSettings;
	int posX, posY;
	hinstance = GetModuleHandle(0);
	wstring nameStr = Str2Wstr(data.title);
	LPCWSTR name = nameStr.c_str();

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = name;
	wc.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wc))
	{
		Log::Error("Regitser Window Falied!");
	}

	int ScrrenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScrrenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (data.fullScreen)
	{
		data.width = ScrrenWidth;
		data.height = ScrrenHeight;
		memset(&dmScrrenSettings, 0, sizeof(dmScrrenSettings));
		dmScrrenSettings.dmSize = sizeof(dmScrrenSettings);
		dmScrrenSettings.dmPelsWidth = (unsigned long)ScrrenWidth;
		dmScrrenSettings.dmPelsHeight = (unsigned long)ScrrenHeight;
		dmScrrenSettings.dmBitsPerPel = 32;
		dmScrrenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScrrenSettings, CDS_FULLSCREEN);

		posX = posY = 0;
	}
	else
	{
		ScrrenWidth = data.width;
		ScrrenHeight = data.height;
		posX = (GetSystemMetrics(SM_CXSCREEN) - ScrrenWidth) / 2 - 200;
		posY = (GetSystemMetrics(SM_CYSCREEN) - ScrrenHeight) / 2 - 100;
	}

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, name, name, WS_OVERLAPPEDWINDOW,
		posX, posY, ScrrenWidth, ScrrenHeight, NULL, NULL, hinstance, NULL);

	if (!hwnd)
	{
		Log::Error("Create Window Failed!");
	}

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	ShowCursor(true);
}

void GameWindow::ShutDown()
{
	DestroyWindow(hwnd);
}

void GameWindow::OnUpdate()
{
	MSG msg = { 0 };

	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void GameWindow::SetEventCallback(const EventCallback& inCallBack)
{
	Get()->bStartEvent = true;
	Get()->eventCallback = inCallBack;
}

void GameWindow::SetVSync(bool bEnabled)
{
	data.bVSync = bEnabled;
}

bool GameWindow::IsVSync()
{
	return Get()->data.bVSync;
}

LRESULT CALLBACK GameWindow::MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(!bStartEvent)
		return DefWindowProc(hwnd, message, wParam, lParam);

	switch (message)
	{
		case WM_CLOSE:
		{
			WindowCloseEvent event;
			eventCallback(event);
			return 0;
		}
		case WM_SIZE:
		{
			int newWidth = LOWORD(lParam);
			int newHeight = HIWORD(lParam);
			WindowResizeEvent event(newWidth, newHeight);
			eventCallback(event);
			return 0;
		}
		case WM_KEYDOWN:
		{
			KeyPressedEvent event((UINT)wParam, 1);
			eventCallback(event);
			return 0;
		}
		case WM_KEYUP:
		{
			KeyReleasedEvent event((UINT)wParam);
			eventCallback(event);

			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			float mouseX = GET_X_LPARAM(lParam);
			float mouseY = GET_Y_LPARAM(lParam);
			MouseKey key = MouseKey::LeftButton;
			MouseButtonPressedEvent event(key, mouseX, mouseY);
			eventCallback(event);
			return 0;
		}

		case WM_LBUTTONUP:
		{
			float mouseX = GET_X_LPARAM(lParam);
			float mouseY = GET_Y_LPARAM(lParam);
			MouseKey key = MouseKey::LeftButton;
			MouseButtonReleasedEvent event(key);
			eventCallback(event);
			return 0;
		}

		case WM_RBUTTONDOWN:
		{
			float mouseX = GET_X_LPARAM(lParam);
			float mouseY = GET_Y_LPARAM(lParam);
			MouseKey key = MouseKey::RightButton;
			MouseButtonPressedEvent event(key, mouseX, mouseY);
			eventCallback(event);
			return 0;
		}

		case WM_RBUTTONUP:
		{
			float mouseX = GET_X_LPARAM(lParam);
			float mouseY = GET_Y_LPARAM(lParam);
			MouseKey key = MouseKey::RightButton;
			MouseButtonReleasedEvent event(key);
			eventCallback(event);
			return 0;
		}

		case WM_MOUSEMOVE:
		{
			float mouseX = GET_X_LPARAM(lParam);
			float mouseY = GET_Y_LPARAM(lParam);
			MouseMovedEvent event(mouseX, mouseY);
			eventCallback(event);
			return 0;
		}

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
}

shared_ptr<GameWindow> GameWindow::Get()
{
	if (nullptr == single)
	{
		single = shared_ptr<GameWindow>(new GameWindow());
	}

	return single;
}

shared_ptr<GameWindow> GameWindow::single = nullptr;