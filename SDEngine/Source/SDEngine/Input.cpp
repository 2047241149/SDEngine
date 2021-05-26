#include"Input.h"
#include "WindowInfo.h"

Input::Input()
{
}

Input::Input(const Input& inputclass)
{
}

Input::~Input()
{
}

bool Input::Init()
{
	mDirectInput = NULL;
	mDirectInputKeyboard = NULL;
	mDirectInputMouse = NULL;
	mMousePosX = 0;
	mMousePosY = 0;

	HR(DirectInput8Create(GHinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mDirectInput, NULL));
	HR(mDirectInput->CreateDevice(GUID_SysKeyboard, &mDirectInputKeyboard, NULL));
	HR(mDirectInputKeyboard->SetDataFormat(&c_dfDIKeyboard));
	HR(mDirectInputKeyboard->SetCooperativeLevel(GHwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE));
	HR(mDirectInput->CreateDevice(GUID_SysMouse, &mDirectInputMouse, NULL));
	HR(mDirectInputMouse->SetDataFormat(&c_dfDIMouse));
	HR(mDirectInputMouse->SetCooperativeLevel(GHwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE));
	return TRUE;

}


void Input::ShutDown()
{
	ReleaseCOM(mDirectInputMouse);
	ReleaseCOM(mDirectInputKeyboard);
	ReleaseCOM(mDirectInput);
}


bool Input::Tick()
{
	bool result;

	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	ProcessInput();
	return true;
}



bool Input::ReadKeyboard()
{
	//read keybord state
	HRESULT result;
	result = mDirectInputKeyboard->GetDeviceState(sizeof(mKeyboardState), (LPVOID)&mKeyboardState);

	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			mDirectInputKeyboard->Acquire();
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

	//read mouse state
	result = mDirectInputMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mMouseState);
   
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			mDirectInputMouse->Acquire();
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
	mMousePosXOffset = mMouseState.lX;
	mMousePosYOffset = mMouseState.lY;
	mMousePosX += mMouseState.lX;
	mMousePosY += mMouseState.lY;

   //mouse pos must be in screen
	if (mMousePosX < 0)
	{
		mMousePosX = 0;
	}
	if (mMousePosY < 0)
	{
		mMousePosY = 0;
	}
	if (mMousePosX > GScreenWidth)
	{
		mMousePosX = GScreenWidth;
	}
	if (mMousePosY > GScreenHeight)
	{
		mMousePosY = GScreenHeight;
	}
}

bool Input::IsEscapePressed()
{
	if (mKeyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}
		return false;
}

bool Input::IsWPressed()
{
	if (mKeyboardState[DIK_W] & 0x80)
	{
		return true;
	}
	return false;
}


bool Input::IsSPressed()
{
	if (mKeyboardState[DIK_S] & 0x80)
	{
		return true;
	}
	return false;
}



bool Input::IsAPressed()
{
	if (mKeyboardState[DIK_A] & 0x80)
	{
		return true;
	}
	return false;
}



bool Input::IsDPressed()
{
	if (mKeyboardState[DIK_D] & 0x80)
	{
		return true;
	}
	return false;
}



bool Input::IsQPressed()
{
	if (mKeyboardState[DIK_Q] & 0x80)
	{
		return true;
	}
	return false;
}


bool Input::IsEPressed()
{
	if (mKeyboardState[DIK_E] & 0x80)
	{
		return true;
	}
	return false;
}

bool Input::IsKeyDown(int key)
{
	if (mKeyboardState[key] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsMouseRightButtuonPressed()
{
	if (mMouseState.rgbButtons[1] & 0x80)
	{
		return true;
	}
	return false;
}

void Input::GetMousePosition(int& MouseX, int& MouseY)
{
	MouseX = mMousePosX;
	MouseY = mMousePosY;
}

void Input::GetMousePositionOffset(int& MouseXOffset, int &MouseYOffset)
{
	MouseXOffset = mMousePosXOffset;
	MouseYOffset = mMousePosYOffset;
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