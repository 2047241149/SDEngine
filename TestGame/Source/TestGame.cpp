
#include "Engine.h"

class TestGame : public Game
{
public:
	TestGame()
	{

	}

	~TestGame()
	{

	}
};

Game* CreateGame()
{
	return new TestGame();
}