#include "ImguiLayer.h"
#include "imgui.h"
#include "ImGuiRender.h"
#include "GameWindow.h"
#include "SDEngine/Common/DirectxCore.h"

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

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup backend capabilities flags
	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	io.ImeWindowHandle = GWindowHwnd;
	
	//imgui key map to win32
	io.KeyMap[ImGuiKey_Tab] = VK_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.KeyMap[ImGuiKey_Home] = VK_HOME;
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Space] = VK_SPACE;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	io.KeyMap[ImGuiKey_KeyPadEnter] = VK_RETURN;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	ImGui_ImplDX11_Init(g_pDevice, g_pDeviceContext);
}

void ImGuiLayer::OnDetach()
{

}

void ImGuiLayer::OnUpdate(float deltaTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGuiIO& io = ImGui::GetIO();
	//io.DisplaySize = ImVec2(GWindowWidth, GWindowHeight);
	RECT rect = { 0, 0, 0, 0 };
	::GetClientRect(GWindowHwnd, &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
	io.DeltaTime = 0.003;
	io.KeyCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (::GetKeyState(VK_MENU) & 0x8000) != 0;
	io.KeySuper = false;

	UpdateMousePos();
	ImGui::NewFrame();
	bool bShow = true;
	ImGui::ShowDemoWindow(&bShow);
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::OnEvent(Event& event)
{
	EventDispatcher eventDispatcher(event);
	eventDispatcher.Dispath<KeyPressedEvent>(BIND_EVENT(ImGuiLayer::OnKeyPressedEvent, this));
	eventDispatcher.Dispath<KeyReleasedEvent>(BIND_EVENT(ImGuiLayer::OnKeyReleasedEvent, this));
	eventDispatcher.Dispath<CharEvent>(BIND_EVENT(ImGuiLayer::OnCharEvent, this));

	eventDispatcher.Dispath<MouseButtonPressedEvent>(BIND_EVENT(ImGuiLayer::OnMouseButtonPressedEvent, this));
	eventDispatcher.Dispath<MouseButtonReleasedEvent>(BIND_EVENT(ImGuiLayer::OnMouseButtonReleasedEvent, this));
	eventDispatcher.Dispath<MouseMovedEvent>(BIND_EVENT(ImGuiLayer::OnMouseMovedEvent, this));
	eventDispatcher.Dispath<MouseScrollEvent>(BIND_EVENT(ImGuiLayer::OnMouseScrollEvent, this));
	
	eventDispatcher.Dispath<KillFocusEvent>(BIND_EVENT(ImGuiLayer::OnKillWindowFocus, this));
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