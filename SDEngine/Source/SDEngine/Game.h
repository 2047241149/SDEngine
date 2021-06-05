#pragma once

#include "CoreMinimal.h"
class Event;
class GameWindow;
class Layer;
class LayerManager;

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

private:
	shared_ptr<GameWindow> window;
	shared_ptr<LayerManager> layerManager;
	bool bRunning = false;
};

Game* CreateGame();