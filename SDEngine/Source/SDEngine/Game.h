#pragma once

#include "CoreMinimal.h"

class GameWindow;

class SD_API Game
{
public:
	Game();
	~Game();

public:
	void Run();

private:
	shared_ptr<GameWindow> window;
};

Game* CreateGame();