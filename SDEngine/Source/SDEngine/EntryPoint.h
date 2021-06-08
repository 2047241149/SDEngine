#pragma once

extern Game* CreateGame();

int main()
{
	Log::Info("Hello World SDEngine");
	auto* game = CreateGame();
	game->Run();
	delete game;
	return 1;
}