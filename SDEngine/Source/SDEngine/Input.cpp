#include"Input.h"
#include "WindowInfo.h"
#include "GameWindow.h"

Input::Input()
{
	Init();
}

Input::~Input()
{
}

bool Input::Init()
{
	directInput = NULL;
	directInputKeyboard = NULL;
	directInputMouse = NULL;
	mousePosX = 0;
	mousePosY = 0;

	HR(DirectInput8Create(GWindowHinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL));
	HR(directInput->CreateDevice(GUID_SysKeyboard, &directInputKeyboard, NULL));
	HR(directInputKeyboard->SetDataFormat(&c_dfDIKeyboard));
	HR(directInputKeyboard->SetCooperativeLevel(GWindowHwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	HR(directInput->CreateDevice(GUID_SysMouse, &directInputMouse, NULL));
	HR(directInputMouse->SetDataFormat(&c_dfDIMouse));
	HR(directInputMouse->SetCooperativeLevel(GWindowHwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	directInputKeyboard->Acquire();
	directInputMouse->Acquire();
	return TRUE;
}

void Input::ShutDown()
{
	ReleaseCOM(directInputMouse);
	ReleaseCOM(directInputKeyboard);
	ReleaseCOM(directInput);
}

bool Input::Tick()
{
	shared_ptr<Input> input = Get();

	bool result;

	result = input->ReadKeyboard();
	if (!result)
	{
		return false;
	}

	result = input->ReadMouse();
	if (!result)
	{
		return false;
	}

	input->ProcessInput();
	return true;
}

bool Input::ReadKeyboard()
{
	//read keybord state
	HRESULT result;
	directInputKeyboard->Poll();
	result = directInputKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			directInputKeyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}


bool Input::ReadMouse()
{
	HRESULT result;
	directInputMouse->Poll();

	//read mouse state
	result = directInputMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState);
   
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			directInputMouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}


void Input::ProcessInput()
{
	mousePosXOffset = mouseState.lX;
	mousePosYOffset = mouseState.lY;
	mousePosX += mouseState.lX;
	mousePosY += mouseState.lY;

    //mouse pos must be in screen
	if (mousePosX < 0)
	{
		mousePosX = 0;
	}

	if (mousePosY < 0)
	{
		mousePosY = 0;
	}

	if (mousePosX > GScreenWidth)
	{
		mousePosX = GScreenWidth;
	}

	if (mousePosY > GScreenHeight)
	{
		mousePosY = GScreenHeight;
	}
}


bool Input::IsKeyDown(EKey keyCode)
{
	shared_ptr<Input> input = Get();

	if (input->keyboardState[int(keyCode)] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsMouseButtuonPressed(EMouse keyCode)
{
	shared_ptr<Input> input = Get();

	if (input->mouseState.rgbButtons[int(keyCode)] & 0x80)
	{
		return true;
	}

	return false;
}

void Input::GetMousePosition(int& MouseX, int& MouseY)
{
	shared_ptr<Input> input = Get();
	MouseX = input->mousePosX;
	MouseY = input->mousePosY;
}

void Input::GetMousePositionOffset(int& MouseXOffset, int &MouseYOffset)
{
	shared_ptr<Input> input = Get();
	MouseXOffset = input->mousePosXOffset;
	MouseYOffset = input->mousePosYOffset;
}

shared_ptr<Input> Input::Get()
{
	if (nullptr == single)
	{
		single = shared_ptr<Input>(new Input());
	}

	return single;
}

shared_ptr<Input> Input::single = nullptr;