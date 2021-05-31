#include "Game.h"
#include "GameWindow.h"
#include "Log.h"
#include "Event/WindowEvent.h"


Game::Game()
{
	window = GameWindow::Create();
}

Game::~Game()
{
}

void Game::Run()
{
	while (true)
	{
		window->OnUpdate();
	}
}