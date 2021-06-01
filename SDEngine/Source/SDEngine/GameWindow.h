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
	GameWindow(const EventCallback& inEvent, const WindowProp& props);
	virtual ~GameWindow();

public:
	void OnUpdate();
	int GetWidth() { return data.width; }
	int GetHeight() { return data.height; }

	//Window attributes
	void SetEventCallback(const EventCallback& callBack);
	void SetVSync(bool bEnabled);
	bool IsVSync();
	static shared_ptr<GameWindow> Create(const EventCallback& inEvent, const WindowProp& props = WindowProp());
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void Init(const EventCallback& inEvent, const WindowProp& props = WindowProp());
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

	EventCallback eventCallback;
	WindowData data;
	HINSTANCE hinstance; //应用实例句柄
	HWND hwnd; //应用窗口句柄
};