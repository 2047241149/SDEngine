#include "Game.h"
#include "GameWindow.h"
#include "Log.h"
#include "Event/WindowEvent.h"
#include "LayerManager.h"
#include "Common/DirectxCore.h"
#include "FPS.h"
#include "Input.h"
#include "GameObject/Camera.h"
#include "EditorUI/ImGuiLayer.h"
#include "Texture/TextureManager.h"

Game* Game::single = nullptr;

Game::Game()
{
	bRunning = true;
	layerManager = shared_ptr<LayerManager>(new LayerManager());
	imguiLayer = make_shared<ImGuiLayer>();

	GGameWindow->SetEventCallback(BIND_EVENT(Game::OnEvent, this));
	GGameWindow->Init();

	GGameWindow->SetVSync(true);
	GDirectxCore->Init(GIsVSync, GIsFullScrren);
	GInput->Init();
	GCamera->SetProjParams(XM_PI / 3.0f, (float)GWindowWidth / (float)GWindowHeight);
	GCamera->SetUIOrthoParams((float)GWindowWidth, (float)GWindowHeight);
	imguiLayer->OnAttach();
}

Game::~Game()
{
	imguiLayer->OnDetach();
}

void Game::OnEvent(Event& event)
{
	GDirectxCore->OnEvent(event);

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
		if (GGameWindow->OnUpdateMsg())
			continue;

		GFPS->Frame();
		GInput->Tick();

		// 3D scene render
		GDirectxCore->BeginSceneRender();

		for (auto it = layerManager->Begin(); it != layerManager->End(); ++it)
		{
			(*it)->OnUpdate(GFPS->GetDeltaTime());
		}
		Update();

		// imgui layer render
		GDirectxCore->SetBackBufferRender();
		imguiLayer->BeginRender();
		for (auto it = layerManager->Begin(); it != layerManager->End(); ++it)
		{
			(*it)->OnImguiRender();
		}
		OnImguiRender();
		imguiLayer->EndRender();

		// end scene
		GDirectxCore->EndScene();
		for (auto it = layerManager->Begin(); it != layerManager->End(); ++it)
		{
			(*it)->End();
		}
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

void Game::Close()
{
	bRunning = false;
}
