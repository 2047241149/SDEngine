#pragma once

#include "CoreMinimal.h"
class Event;
class GameWindow;
class Layer;
class LayerManager;
class ImGuiLayer;

class SD_API Game
{
public:
	Game();
	~Game();

public:
	void Run();
	void OnEvent(Event& event);
	bool OnClose(Event& event);
	
public:
	void PushLayer(shared_ptr<Layer> layer);
	void PopLayer(shared_ptr<Layer> layer);
	virtual void Update() {};
	virtual void OnImguiRender() {};
	void Close();
	void SetEventBlock(bool block);

public:
	static Game* Get() { return single; }
	static void SetupSingle(Game* other) { single = other; }

private:
	shared_ptr<GameWindow> window;
	shared_ptr<LayerManager> layerManager;
	shared_ptr<ImGuiLayer> imguiLayer;
	bool bRunning = false;

private:
	static Game* single;
};

Game* CreateGame();

#define GGame Game::Get()