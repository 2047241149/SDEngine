#include "EditorLayer.h"
#include "ImguiUtil.h"
#include "TestScript.h"
#include "EditorStyle/EditorStyle.h"

EditorLayer::EditorLayer():
	Layer("ExmPlayer"),
	bUseEditorMode(true)
{
}

void EditorLayer::OnAttach()
{
	rt = make_shared<RenderTexture>(GViewportWidth, GViewportHeight);
	scene = make_shared<Scene>();

	pureColorShader = ShaderLibrary::LoadGetShader("Content/PureColorShader.fx");
	baseDiffuse = make_shared<Texture>("Content/rustediron2_basecolor.png");
	material = make_shared<Material>(pureColorShader);
	material->SetTexture("BaseTexture", baseDiffuse);
	material->SetTextureSampler("SampleWrapLinear", TextureSampler::BilinearFliterClamp);

	meshActor = scene->CreateActor("meshActor");
	auto& meshCpt = meshActor.AddComponent<MeshComponent>("Content\\sphere.FBX");
	meshActor.AddComponent<ScriptComponent>("TestScript").Bind<TestScript>();
	meshCpt.SetMaterial(material);

	secondCameraActor = scene->CreateActor("secondCamera");
	auto& secondCameraCpt = secondCameraActor.AddComponent<CameraComponent>();
	secondCameraCpt.bPrimary = !bUseEditorMode;
	
	scenePanel.SetScene(scene);
	GDirectxCore->SetBeginSceneColor(XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
	GEditorStyle->Init();
};

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnTick(float deltaTime)
{
	PROFILE_FUNC();

	if (bViewportFouces && bViewportHover && bUseEditorMode)
	{
		editorCamera.Tick(deltaTime);
	}

	rt->SetRenderTarget(1.0f, 1.0f, 0.0f, 1.0f);
	GDirectxCore->RecoverDefaultDSS();
	GDirectxCore->RecoverDefualtRS();

	{
		PROFILE_SCOPE("draw");
		if (bUseEditorMode)
		{
			scene->OnTickEditor(deltaTime, &editorCamera);
		}
		else
		{
			scene->OnTickRuntime(deltaTime);
		}

	}
};

void EditorLayer::OnImguiRender() 
{
	OnDockSpaceUI();
	OnMenuUI();
	OnRenderStatisticsUI();
	OnGameWindowUI();
	scenePanel.OnImguiRender();
}

void EditorLayer::OnDockSpaceUI()
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
	ImGuiStyle& style = ImGui::GetStyle();
	float minWindowSize = style.WindowMinSize.x;
	style.WindowMinSize.x = 200.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	style.WindowMinSize.x = minWindowSize;
}

void EditorLayer::OnEvent(Event& event)
{
	Log::Info(event);
}

void EditorLayer::End()
{
	Profile::ClearData();
	GDirectxCore->End();
}

void EditorLayer::OnMenuUI()
{
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
}

void EditorLayer::OnRenderStatisticsUI()
{
	// Profile && render statistics
	ImGui::Begin("Profile");
	ImGui::Text("FPS: %d  ", FPS::Get()->GetFPS());

	const vector<ProfileResult>& profiles = Profile::GetProfileResults();
	for (auto& it : profiles)
	{
		ImGui::Text("%s: %0.4fms", it.name.c_str(), it.time);
	}

	const RenderStatistics& statistics = GDirectxCore->GetStatistics();
	ImGui::Text("DrawCall: %d  ", statistics.drawCount);
	ImGui::Text("TriCount: %d  ", statistics.triCount);

	if (ImGui::Checkbox("IsEditorMode", &bUseEditorMode))
	{
		secondCameraActor.GetComponent<CameraComponent>().bPrimary = !bUseEditorMode;
	}

	ImGui::End();
}

void EditorLayer::OnGameWindowUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("viewport");

	bViewportFouces = ImGui::IsWindowFocused();
	bViewportHover = ImGui::IsWindowHovered();
	GGame->SetEventBlock(!bViewportFouces && !bViewportHover);
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	int rtWidth = rt->GetWidth();
	int rtHeight = rt->GetHeight();

	if (rtWidth != viewportSize.x || rtHeight != viewportSize.y)
	{
		rt->Resize((int)viewportSize.x, (int)viewportSize.y);
	}

	ImGui::Image(rt->GetSRV(), ImVec2((float)rtWidth, (float)rtHeight));
	ImGui::End();
	ImGui::PopStyleVar();
}