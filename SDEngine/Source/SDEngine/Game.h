#pragma once

#include "CoreMinimal.h"

class SD_API Game
{
public:
	Game();
	~Game();

public:
	void Run();
};

Game* CreateGame();