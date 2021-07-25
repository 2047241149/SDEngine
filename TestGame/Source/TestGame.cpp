
#include "Engine.h"
#include "imgui/imgui.h"

class ExmPlayer : public Layer
{

public:
	void OnAttach() override 
	{
	};

	void OnDetach() override 
	{
	}

	void OnEvent(Event& event) override
	{
	}
};

class TestGame : public Game
{
public:
	TestGame():
		Game()
	{
		PushLayer(shared_ptr<Layer>(new ExmPlayer()));
	}

	~TestGame()
	{
	}
};

Game* CreateGame()
{
	return new TestGame();
}