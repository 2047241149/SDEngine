#include "GraphicsSystem.h"
#include "GameObject/Camera.h"
#include "Input.h"
#include "SceneManager.h"
#include "WindowInfo.h"
#include "TiledDefferedSceneRender.h"
#include "ClusterDefferedSceneRender.h"

GraphicsSystem::GraphicsSystem()
{
	Init();
}

GraphicsSystem::~GraphicsSystem()
{
	CloseDebugConsole();
}

GraphicsSystem::GraphicsSystem(const GraphicsSystem&other)
{

}

bool GraphicsSystem::Init()
{
	//Init D3d11 Core Object
	GDirectxCore->Init(VSYNC_ENABLE, FULL_SCREEN);

	//Init Console Debug Windows
	InitDebugConsole();

	//Init SceneManager
	GSceneManager->Init();

	//Init Scene Render
	//sceneRender = shared_ptr<TiledDefferedSceneRender>(new TiledDefferedSceneRender());
	sceneRender = shared_ptr<ClusterDefferedSceneRender>(new ClusterDefferedSceneRender());
	sceneRender->PreRender();
	return true;
}


bool GraphicsSystem::Tick(float deltaTime)
{
	if (Input::IsKeyDown(EKey::KEY_7))
	{
		bSSAO = false;
	}
	else
	{
		bSSAO = true;
	}

	GSceneManager->Tick(deltaTime);
	sceneRender->Tick(deltaTime);
	return true;
}

void GraphicsSystem::Render()
{
	sceneRender->Render();
}

void GraphicsSystem::InitDebugConsole()
{
	AllocConsole();
	FILE* pf;
	freopen_s(&pf, "CONOUT$", "w", stdout);   // �ض������
}

void GraphicsSystem::CloseDebugConsole()
{
	FreeConsole();
}