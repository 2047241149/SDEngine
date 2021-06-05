#pragma once

extern Game* CreateGame();

int main()
{
	Log::Error("Hello World");
	Log::Info("Hello World  {1}_{0}", "engine", "sd");
	auto* game = CreateGame();
	game->Run();
	delete game;
	return 1;
}