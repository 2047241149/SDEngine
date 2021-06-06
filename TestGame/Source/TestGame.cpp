
#include "Engine.h"

class ExmPlayer : public Layer
{
public:
	ExmPlayer() :
		Layer("ExmPlayer")
	{

	}

public:
	virtual void OnAttach() override 
	{
		Log::Info("OnAttach{0}", layerName);
	};

	virtual void OnDetach() override 
	{
		Log::Info("OnDetach{0}", layerName);
	}
	virtual void OnUpdate(float deltaTime) override 
	{
		//Log::Info("OnUpdate{0}", deltaTime);
	};


	virtual void OnEvent(Event& event) override 
	{
		//Log::Info("OnUpdate{0}", event);
	}
};

class TestGame : public Game
{
public:
	TestGame():
		Game()
	{
		PushLayer(shared_ptr<Layer>(new ExmPlayer()));
		PushLayer(shared_ptr<Layer>(new ImGuiLayer()));
	}

	~TestGame()
	{

	}
};

Game* CreateGame()
{
	return new TestGame();
}