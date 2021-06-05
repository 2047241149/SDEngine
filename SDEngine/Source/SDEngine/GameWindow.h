#pragma once
#include "Event/EventBase.h"


struct WindowProp 
{
	string tile;
	int width;
	int height;

	WindowProp(const string& inTitle = "SDEngine",
		int inWidth = 1024, int inHeight = 768):
		tile(inTitle),
		width(inWidth),
		height(inHeight)
	{
	}
};


class GameWindow
{
public:
	using EventCallback = std::function<void(Event&)>;
	GameWindow();
	virtual ~GameWindow();

public:
	void Init(const WindowProp& props = WindowProp());
	void OnUpdate();
	
	//Window attributes
	int GetWidth() { return data.width; }
	int GetHeight() { return data.height; }
	HWND GetHwnd() { return hwnd; }
	HINSTANCE GetWindowHinstance() { return hinstance; }
	bool IsVSync();
	bool IsFullScreen() { return data.fullScreen; }
	void SetEventCallback(const EventCallback& callBack);
	void SetVSync(bool bEnabled);
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	//static function
	static shared_ptr<GameWindow> Get();

private:
	void InitWindow();
	void ShutDown();

private:
	struct WindowData
	{
		string title;
		int width, height;
		bool bVSync;
		bool fullScreen;
	};

	bool bStartEvent;
	EventCallback eventCallback;
	WindowData data;

	//app hinstance
	HINSTANCE hinstance;

	//window hwnd
	HWND hwnd;

private:
	static shared_ptr<GameWindow> single;
};

#define GGameWindow (GameWindow::Get())
#define GWindowWidth (GGameWindow->GetWidth())
#define GWindowHeight (GGameWindow->GetHeight())
#define GWindowHwnd (GGameWindow->GetHwnd())
#define GWindowHinstance (GGameWindow->GetWindowHinstance())
#define GIsVSync (GGameWindow->IsVSync())
#define GIsFullScrren (GGameWindow->IsFullScreen())