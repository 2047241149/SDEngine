#pragma once

extern Game* CreateGame();

int main()
{
	printf("Hello SDEngine");
	auto* game = CreateGame();
	game->Run();
	delete game;
	return 0;
}