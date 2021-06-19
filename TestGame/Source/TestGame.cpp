
#include "Engine.h"
#include "imgui/imgui.h"

class ExmPlayer : public Layer
{
private:
	float cameraMoveSpeed = 3.0f;
	float cameraRotateSpeed = 0.5f;
	shared_ptr<VertexPixelShader> pureColorShader;
	shared_ptr<GameObject> cube;
	float surfaceColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};

public:
	ExmPlayer() :
		Layer("ExmPlayer")
	{
		pureColorShader = shared_ptr<VertexPixelShader>(new
			VertexPixelShader(L"Resource/PureColorShader.fx", L"Resource/PureColorShader.fx"));
		
		cube = shared_ptr<GameObject>(new GameObject());
		shared_ptr<Mesh> sphereMesh = shared_ptr<Mesh>(new Mesh("Resource\\sphere.FBX"));
		cube->SetMesh(sphereMesh);
		cube->m_pTransform->localPosition = XMFLOAT3(0.0f, 1.0f, 0.0f);
		cube->m_pTransform->localScale = XMFLOAT3(2.0, 2.0, 2.0);
	}

private:
	void UpdateCamera(float deltaTime)
	{
		int mouseXOffset, mouseYOffset;
		static float rotateY = 0.0f;
		int fps = GFPS->GetFPS();
		GInput->GetMousePositionOffset(mouseXOffset, mouseYOffset);

		if (GInput->IsMouseButtuonPressed(EMouse::Right) && fps >= 5 && fps <= 1000000)
		{
			if (GInput->IsKeyDown(EKey::W))
			{
				GCamera->Walk(deltaTime * cameraMoveSpeed);
			}
			else if (GInput->IsKeyDown(EKey::S))
			{
				GCamera->Walk(-deltaTime * cameraMoveSpeed);
			}

			if (GInput->IsKeyDown(EKey::A))
			{
				GCamera->Strafe(-deltaTime * cameraMoveSpeed);
			}
			else if (GInput->IsKeyDown(EKey::D))
			{
				GCamera->Strafe(deltaTime*cameraMoveSpeed);
			}

			if (GInput->IsKeyDown(EKey::Q))
			{
				GCamera->UpDown(-deltaTime * cameraMoveSpeed);
			}
			else if (GInput->IsKeyDown(EKey::E))
			{
				GCamera->UpDown(deltaTime*cameraMoveSpeed);
			}

			if (rotateY <= 90.0f && rotateY >= -90.0f)
			{
				rotateY = (float)mouseYOffset*deltaTime;
				GCamera->Pitch((float)mouseYOffset * deltaTime* cameraRotateSpeed);
			}

			GCamera->RotateY((float)-mouseXOffset*deltaTime* cameraRotateSpeed);
		}

		GCamera->UpdateViewMatrix();
	}

public:
	void OnAttach() override 
	{
		GDirectxCore->SetBeginSceneColor(XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
	};

	void OnDetach() override 
	{
	}

	void OnUpdate(float deltaTime) override 
	{
		UpdateCamera(deltaTime);
		GDirectxCore->SetBackBufferRender();
		GDirectxCore->RecoverDefaultDSS();
		pureColorShader->SetMatrix("World", cube->GetWorldMatrix());
		pureColorShader->SetMatrix("View", GCamera->GetViewMatrix());
		pureColorShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
		pureColorShader->SetFloat4("surfaceColor", XMFLOAT4(surfaceColor[0], surfaceColor[1], surfaceColor[2], surfaceColor[3]));
		pureColorShader->Apply();
		cube->RenderMesh();
	};

	void OnImguiRender() override 
	{
		//bool bShow = true;
		//ImGui::ShowDemoWindow(&bShow);

		ImGui::ColorEdit4("Pick", surfaceColor);
		ImGui::Begin("Profile");
		ImGui::Text("FPS: %d  ", FPS::Get()->GetFPS());
		ImGui::End();
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