﻿#include "Game.h"
#include "GameWindow.h"
#include "Log.h"
#include "Event/WindowEvent.h"
#include "LayerManager.h"
#include "Common/DirectxCore.h"
#define BIND_EVENT(func, object) (std::bind(&func, object, std::placeholders::_1))

Game::Game()
{
	bRunning = true;
	layerManager = shared_ptr<LayerManager>(new LayerManager());

	GGameWindow->Init();
	GGameWindow->SetEventCallback(BIND_EVENT(Game::OnEvent, this));

	GDirectxCore->Init(GIsVSync, GIsFullScrren);
}

Game::~Game()
{
}

void Game::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispath<WindowCloseEvent>(BIND_EVENT(Game::OnClose, this));
	Log::Info(event);

	for (auto it = layerManager->Begin(); it != layerManager->End(); ++it)
	{
		(*it)->OnEvent(event);
	}
}

bool Game::OnClose(Event& event)
{
	bRunning = false;
	return true;
}

void Game::Run()
{
	while (bRunning)
	{
		GGameWindow->OnUpdate();

		for (auto it = layerManager->Begin(); it != layerManager->End(); ++it)
		{
			(*it)->OnUpdate();
		}

		GDirectxCore->BeginScene(1.0, 0.0, 0.0, 1.0);
		GDirectxCore->EndScene();
	}
}

void Game::PushLayer(shared_ptr<Layer> layer)
{
	layerManager->PushLayer(layer);
}

void Game::PopLayer(shared_ptr<Layer> layer)
{
	layerManager->PopLayer(layer);
}