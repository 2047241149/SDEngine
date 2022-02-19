#include "ImguiLayer.h"
#include "GameWindow.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "SDEngine/Common/DirectxCore.h"
#include "ImGuizmo.h"

ImGuiLayer::ImGuiLayer() :
	Layer("ImGuiLayer")
{

}

ImGuiLayer::~ImGuiLayer()
{

}

void ImGuiLayer::OnAttach()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup backend capabilities flags
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(GWindowHwnd);
	ImGui_ImplDX11_Init(g_pDevice, g_pDeviceContext, GMainViewRTV);
}

void ImGuiLayer::OnDetach()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiLayer::BeginRender()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

void ImGuiLayer::EndRender()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void ImGuiLayer::OnEvent(Event& event)
{
	if (bBlockEvent)
	{
		if (ImGui::GetCurrentContext() != NULL)
		{
			ImGuiIO& io = ImGui::GetIO();
			event.bHandled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.bHandled |= event.IsInCategory(EventCategoryKeybord) & io.WantCaptureKeyboard;
		}
	}
	else
	{
		event.bHandled = false;
	}

}

void ImGuiLayer::SetImguiKeyCode(UINT keyCode, int taregetValue)
{
	ImGuiIO& io = ImGui::GetIO();
	if (keyCode < 255)
	{
		io.KeysDown[keyCode] = taregetValue;
	}
}

void ImGuiLayer::UpdateMousePos()
{
	ImGuiIO& io = ImGui::GetIO();
	// Set mouse position
	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	POINT pos;
	if (HWND active_window = ::GetForegroundWindow())
		if (active_window == GWindowHwnd || ::IsChild(active_window, GWindowHwnd))
			if (::GetCursorPos(&pos) && ::ScreenToClient(GWindowHwnd, &pos))
				io.MousePos = ImVec2((float)pos.x, (float)pos.y);
}


bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& event)
{
	SetImguiKeyCode(event.GetKeyCode(), 1);
	return true;
}

bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& event)
{
	SetImguiKeyCode(event.GetKeyCode(), 0);
	return true;
}

bool ImGuiLayer::OnCharEvent(CharEvent& event)
{
	ImGuiIO& io = ImGui::GetIO();
	UINT keyCode = event.GetKeyCode();
	if (keyCode > 0 && keyCode < 0x10000)
		io.AddInputCharacterUTF16((unsigned short)keyCode);

	return true;
}

bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& event)
{
	int button = 0;
	switch (event.GetMouseKey())
	{
		case MouseKey::LeftButton:
			button = 0;
			break;
		case MouseKey::RightButton:
			button = 1;
			break;
		case MouseKey::MiddleButton:
			button = 2;
			break;
		default:
			button = 0;
			break;
	}

	if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
		::SetCapture(GWindowHwnd);

	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[button] = true;
	return true;
}

bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event)
{
	int button = 0;
	switch (event.GetMouseKey())
	{
	case MouseKey::LeftButton:
		button = 0;
		break;
	case MouseKey::RightButton:
		button = 1;
		break;
	case MouseKey::MiddleButton:
		button = 2;
		break;
	default:
		button = 0;
		break;
	}

	if (!ImGui::IsAnyMouseDown() && ::GetCapture() == GWindowHwnd)
		::ReleaseCapture();

	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[button] = false;
	return true;
}

bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& event)
{
	/*float mousePosX = event.GetMouseX();
	float mousePosY = event.GetMouseY();
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos.x = mousePosX;
	io.MousePos.y = mousePosY;*/
	return true;
}

bool ImGuiLayer::OnMouseScrollEvent(MouseScrollEvent& event)
{
	float mouseWheelOffset = event.GetMouseY();
	float mouseWheelOffsetH = event.GetMouseX();
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheel += mouseWheelOffset;
	io.MouseWheelH += mouseWheelOffsetH;
	return true;
}

bool ImGuiLayer::OnKillWindowFocus(KillFocusEvent& event)
{
	ImGuiIO& io = ImGui::GetIO();
	memset(io.KeysDown, 0, sizeof(io.KeysDown));
	return true;
}

void ImGuiLayer::SetEventBlock(bool block)
{
	bBlockEvent = block;
}