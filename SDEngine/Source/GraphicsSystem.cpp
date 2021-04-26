#include "GraphicsSystem.h"
#include "GameObject/Camera.h"
#include "Input.h"
#include "SceneManager.h"
#include "WindowInfo.h"
#include "TiledDefferedSceneRender.h"


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
	GDirectxCore->Init(VSYNC_ENABLE, FULL_SCREEN, SCREEN_NEAR, SCREEN_FAR);

	//Init Console Debug Windows
	InitDebugConsole();

	//Init Input
	GInput->Init();

	//Init SceneManager
	GSceneManager->Init();

	//Init Scene Render
	sceneRender = shared_ptr<TiledDefferedSceneRender>(new TiledDefferedSceneRender());
	sceneRender->PreRender();
	return true;
}


bool GraphicsSystem::Tick(float deltaTime)
{
	bool result;
	int mouseXOffset, mouseYOffset;
	static float rotateY = 0.0f;
	
	//Tick Input
	result = GInput->Tick();
	if (!result)
	{
		return false;
	}
	
	GFPS->Frame();
	float currentTime = GFPS->GetTime();
	GInput->GetMousePositionOffset(mouseXOffset, mouseYOffset);
	int fps = GFPS->GetFPS();

	//����Ҽ����ڰ��µ�״̬���ܽ��У������ƶ�����ǰ���ƶ�������ת�Ĳ�����
	if (GInput->IsMouseRightButtuonPressed()&& fps >=5 && fps <=1000000)
	{
		//"W","S"������
		if (GInput->IsWPressed())
		{
			GCamera->Walk(deltaTime*CAMERA_SPEED);
		}
		else if (GInput->IsSPressed())
		{
			GCamera->Walk(-deltaTime*CAMERA_SPEED);
		}

		//"A","D"������
		if (GInput->IsAPressed())
		{
			GCamera->Strafe(-deltaTime*CAMERA_SPEED);
		}
		else if (GInput->IsDPressed())
		{
			GCamera->Strafe(deltaTime*CAMERA_SPEED);
		}

		//"Q","E"������
		if (GInput->IsQPressed())
		{
			GCamera->UpDown(-deltaTime*CAMERA_SPEED);
		}
		else if (GInput->IsEPressed())
		{
			GCamera->UpDown(deltaTime*CAMERA_SPEED);
		}

		//�����ӽ����µ���ת(���տ�ʼ����ת�Ƕ�������90��֮��)
		if (rotateY <= 90.0f && rotateY >= -90.0f)
		{
			rotateY += (float)mouseYOffset*deltaTime;
			GCamera->Pitch((float)mouseYOffset*deltaTime*2.0f);
		}
	
		//�����ӽ����ҵ���ת
		GCamera->RotateY((float)mouseXOffset*deltaTime*2.0f);
	}

	//TODO:refactor to imgui control
	if (GInput->IsKeyDown(DIK_1))
	{
		GDirectxCore->RecoverDefualtRS();
	}
	if (GInput->IsKeyDown(DIK_2))
	{
		GDirectxCore->TurnOnWireFrameRender();
	}

	if (GInput->IsKeyDown(DIK_7))
	{
		bSSAO = false;
	}
	else
	{
		bSSAO = true;
	}

	//�������ESC�����ƻ�����
	if (GInput->IsEscapePressed())
	{
		DestroyWindow(GHwnd);
	}

	GCamera->UpdateViewMatrix();

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