

#include "Engine.h"
#include "SDEngine/EntryPoint.h"
#include "imgui/imgui.h"
#include "EditorLayer.h"


class SDEditor : public Game
{
public:
	SDEditor():
		Game()
	{
		PushLayer(shared_ptr<Layer>(new EditorLayer()));
	}

	~SDEditor()
	{
	}
};

Game* CreateGame()
{
	return new SDEditor();
}