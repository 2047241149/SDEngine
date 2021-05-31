#include "GameWindow.h"
#include "Common/CommonFunction.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_KEYDOWN:
	{
		if ((unsigned int)wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		return 0;
	}

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);;
	}
}

GameWindow::GameWindow(const WindowProp& props)
{
	Init(props);
}

GameWindow::~GameWindow()
{

}

void GameWindow::Init(const WindowProp& props)
{
	data.width = props.width;
	data.height = props.height;
	data.title = props.tile;
	data.bVSync = false;
	data.fullScreen = false;
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

void GameWindow::OnUpdate()
{

}

void GameWindow::SetEventCallback(const EventCallback& callBack)
{
	data.callback = callBack;
}

void GameWindow::SetVSync(bool bEnabled)
{
	data.bVSync = bEnabled;
}

bool GameWindow::IsVSync()
{
	return data.bVSync;
}

shared_ptr<GameWindow> GameWindow::Create(const WindowProp& props)
{
	return shared_ptr<GameWindow>(new GameWindow(props));
}