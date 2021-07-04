#pragma once

extern Game* CreateGame();

int main()
{
	Log::Info("Hello World SDEngine");
	auto* game = CreateGame();
	Game::SetupSingle(game);
	game->Run();
	delete game;
	return 1;
}