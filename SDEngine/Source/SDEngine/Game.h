#pragma once

#include "CoreMinimal.h"
class Event;

class GameWindow;

class SD_API Game
{
public:
	Game();
	~Game();

public:
	void Run();
	void OnEvent(Event& event);

private:
	shared_ptr<GameWindow> window;
};

Game* CreateGame();