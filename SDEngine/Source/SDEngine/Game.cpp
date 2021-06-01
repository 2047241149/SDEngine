#include "Game.h"
#include "GameWindow.h"
#include "Log.h"
#include "Event/WindowEvent.h"


Game::Game()
{
	window = GameWindow::Create(std::bind(&Game::OnEvent, this, std::placeholders::_1));
}

Game::~Game()
{
}

void Game::OnEvent(Event& event)
{
	Log::Info(event);
}

void Game::Run()
{
	while (true)
	{
		window->OnUpdate();
	}
}