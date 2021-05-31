#pragma once
#include "Event/Event.h"


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
	GameWindow(const WindowProp& props);
	virtual ~GameWindow();

public:
	void OnUpdate();
	int GetWidth() { return data.width; }
	int GetHeight() { return data.height; }

	//Window attributes
	void SetEventCallback(const EventCallback& callBack);
	void SetVSync(bool bEnabled);
	bool IsVSync();
	static shared_ptr<GameWindow> Create(const WindowProp& props = WindowProp());
	
private:
	void Init(const WindowProp& props);
	void InitWindow();

private:
	struct WindowData
	{
		string title;
		int width, height;
		bool bVSync;
		bool fullScreen;
		EventCallback callback;
	};

	WindowData data;
	HINSTANCE hinstance; //应用实例句柄
	HWND hwnd; //应用窗口句柄
};