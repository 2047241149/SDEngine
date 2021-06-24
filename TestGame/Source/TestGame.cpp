
#include "Engine.h"
#include "imgui/imgui.h"

class ExmPlayer : public Layer
{
private:
	float cameraMoveSpeed = 3.0f;
	float cameraRotateSpeed = 0.5f;
	shared_ptr<VertexPixelShader> pureColorShader;
	shared_ptr<GameObject> cube;
	shared_ptr<Texture> baseDiffuse;
	shared_ptr<Material> material;
	float surfaceColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};

public:
	ExmPlayer() :
		Layer("ExmPlayer")
	{
		pureColorShader = ShaderLibrary::LoadGetShader("Resource/PureColorShader.fx");
		baseDiffuse = make_shared<Texture>("Resource/rustediron2_basecolor.png");
		material = make_shared<Material>(pureColorShader);
		material->SetTexture("BaseTexture", baseDiffuse);
		material->SetTextureSampler("SampleWrapLinear", TextureSampler::BilinearFliterClamp);

		cube = make_shared<GameObject>();
		shared_ptr<Mesh> sphereMesh = make_shared<Mesh>("Resource\\sphere.FBX");
		sphereMesh->SetMaterial(material);
		cube->SetMesh(sphereMesh);
		cube->m_pTransform->localPosition = XMFLOAT3(0.0f, 1.0f, 0.0f);
		cube->m_pTransform->localScale = XMFLOAT3(2.0, 2.0, 2.0);
	}

private:
	void UpdateCamera(float deltaTime)
	{
		PROFILE_FUNC();
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
		PROFILE_FUNC();
		UpdateCamera(deltaTime);
		GDirectxCore->SetBackBufferRender();
		GDirectxCore->RecoverDefaultDSS();
		GDirectxCore->RecoverDefualtRS();
		material->SetFloat4("surfaceColor", XMFLOAT4(surfaceColor[0], surfaceColor[1], surfaceColor[2], surfaceColor[3]));
		
		{
			PROFILE_SCOPE("draw");
			cube->DrawMesh();
		}
	};

	void OnImguiRender() override 
	{
		bool bShow = true;
		ImGui::ShowDemoWindow(&bShow);
		ImGui::ColorEdit4("Pick", surfaceColor);
		ImGui::Begin("Profile");
		ImGui::Text("FPS: %d  ", FPS::Get()->GetFPS());

		const vector<ProfileResult>& profiles = Profile::GetProfileResults();
		for (auto& it : profiles)
		{
			ImGui::Text("%s: %0.4fms", it.name.c_str(), it.time);
		}

		ImGui::End();
	}

	void OnEvent(Event& event) override 
	{

	}

	void End() override
	{
		Profile::ClearData();
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