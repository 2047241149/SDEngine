#include "EditorLayer.h"

EditorLayer::EditorLayer():
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
	rt = make_shared<RenderTexture>(GViewportWidth, GViewportHeight);
}


void EditorLayer::UpdateCamera(float deltaTime)
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

		GCamera->RotateY((float)-mouseXOffset * deltaTime* cameraRotateSpeed);
		}

		GCamera->UpdateViewMatrix();
	}


void EditorLayer::OnAttach()
{
	GDirectxCore->SetBeginSceneColor(XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
};

void EditorLayer::OnDetach()
{

}

void EditorLayer::OnUpdate(float deltaTime)
{
	PROFILE_FUNC();
	UpdateCamera(deltaTime);
	rt->SetRenderTarget(1.0f, 1.0f, 0.0f, 1.0f);
	GDirectxCore->RecoverDefaultDSS();
	GDirectxCore->RecoverDefualtRS();
	material->SetFloat4("surfaceColor", XMFLOAT4(surfaceColor[0], surfaceColor[1], surfaceColor[2], surfaceColor[3]));

	{
		PROFILE_SCOPE("draw");
		cube->DrawMesh();
	}
};

void EditorLayer::OnImguiRender() 
{
	//dock space
	static bool dockSpaceOpen = true;;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);

	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::MenuItem("Exit"))
				GGame->Close();

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();

	// Profile
	//bool bShow = true;
	//ImGui::ShowDemoWindow(&bShow);
	ImGui::ColorEdit4("Pick", surfaceColor);
	ImGui::Begin("Profile");
	ImGui::Text("FPS: %d  ", FPS::Get()->GetFPS());

	const vector<ProfileResult>& profiles = Profile::GetProfileResults();
	for (auto& it : profiles)
	{
		ImGui::Text("%s: %0.4fms", it.name.c_str(), it.time);
	}

	ImGui::End();

	// render statistics
	ImGui::Begin("statistics");
	const RenderStatistics& statistics = GDirectxCore->GetStatistics();
	ImGui::Text("DrawCall: %d  ", statistics.drawCount);
	ImGui::Text("TriCount: %d  ", statistics.triCount);
	ImGui::Image(rt->GetSRV(), ImVec2(GViewportWidth / 1.5f, GViewportHeight / 1.5f));
	ImGui::End();
}

void EditorLayer::OnEvent(Event& event)
{

}

void EditorLayer::End()
{
	Profile::ClearData();
	GDirectxCore->End();
}