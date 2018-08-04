#include"Input.h"


Input::Input(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight)
{
	Initialize(hInstance, hwnd, screenWidth, screenHeight);
}

Input::Input(const Input& inputclass)
{

}

Input::~Input()
{

}


bool Input::Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight)
{

	//置空指针
	mDirectInput = NULL;
	mDirectInputKeyboard = NULL;
	mDirectInputMouse = NULL;


	//初始化mScreenWidth,mScreenHeight
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;
    
	//初始化mMosuePosX  mMousePosY
	mMousePosX = 0;
	mMousePosY = 0;

	//第一,-------------创建DirectInput接口,此接口可以用来初始化DirectInputDevice接口------------------
	HR(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mDirectInput, NULL));


	//第二,-------------用DirectInput接口来创建DirectInputDevice接口 DirectInputKeyboard,并且设置好各种参数----------------
	HR(mDirectInput->CreateDevice(GUID_SysKeyboard, &mDirectInputKeyboard, NULL));

	//设置键盘设备接口的数据格式(使用系统预定义的格式)
	HR(mDirectInputKeyboard->SetDataFormat(&c_dfDIKeyboard));

	//设置键盘设备接口的合作等级,键盘输入不与其它程序分享
	HR(mDirectInputKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE));

	//如果键盘设置好了，获取键盘控制使用权
//	HR(mDirectInputKeyboard->Acquire());

	//---------------第三, 用DirectInput接口来创建DirectInputDevice接口 DirectInputMouse,并且设置好各种参数---------------------

	HR(mDirectInput->CreateDevice(GUID_SysMouse, &mDirectInputMouse, NULL));

	//设置鼠标设备接口的数据格式(使用系统预定义的格式)
	HR(mDirectInputMouse->SetDataFormat(&c_dfDIMouse));

	//设置鼠标设备接口的合作等级,鼠标输入不与其它程序分享
	HR(mDirectInputMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE));

	//如果鼠标设置好了，获取鼠标控制使用权
	//HR(mDirectInputMouse->Acquire());

	return TRUE;

}


void Input::ShutDown()
{
	//释放DirectInputMouse
	ReleaseCOM(mDirectInputMouse);

	//释放mDirectInputKeyboard
	ReleaseCOM(mDirectInputKeyboard);

	//释放DirectInput
	ReleaseCOM(mDirectInput);
}


bool Input::Frame()
{
	bool result;

	//读取键盘的现有状态
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	//读取鼠标的现有状态
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	//处理在键盘那和鼠标的改变
	ProcessInput();
	return true;
}



bool Input::ReadKeyboard()
{
	HRESULT result;
	//读取键盘状态
	result = mDirectInputKeyboard->GetDeviceState(sizeof(mKeyboardState), (LPVOID)&mKeyboardState);

	if (FAILED(result))
	{
		//如果是键盘失去集中点或者无法获取控制权,则重新获取控制权
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))  //或者不是且
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

	//读取鼠标状态
	result = mDirectInputMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mMouseState);
   
	if (FAILED(result))
	{
		//如果是鼠标失去集中点或者无法获取控制权,则重新获取控制权
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))//或者不是且
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
	//更新鼠标的偏移量
	mMousePosXOffset = mMouseState.lX;
	mMousePosYOffset = mMouseState.lY;

	//更新鼠标的位置(每帧都进行更新)
	mMousePosX += mMouseState.lX;
	mMousePosY += mMouseState.lY;

   //确保鼠标在屏幕之内
	if (mMousePosX < 0)
	{
		mMousePosX = 0;
	}
	if (mMousePosY < 0)
	{
		mMousePosY = 0;
	}
	if (mMousePosX > mScreenWidth)
	{
		mMousePosX = mScreenWidth;
	}
	if (mMousePosY > mScreenHeight)
	{
		mMousePosY = mScreenHeight;
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

//所谓的按下就是前一次处于up状态，下一次是down状态
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
	//如果右键按下
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