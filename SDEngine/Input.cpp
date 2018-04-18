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

	//�ÿ�ָ��
	mDirectInput = NULL;
	mDirectInputKeyboard = NULL;
	mDirectInputMouse = NULL;


	//��ʼ��mScreenWidth,mScreenHeight
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;
    
	//��ʼ��mMosuePosX  mMousePosY
	mMousePosX = 0;
	mMousePosY = 0;

	//��һ,-------------����DirectInput�ӿ�,�˽ӿڿ���������ʼ��DirectInputDevice�ӿ�------------------
	HR(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mDirectInput, NULL));


	//�ڶ�,-------------��DirectInput�ӿ�������DirectInputDevice�ӿ� DirectInputKeyboard,�������úø��ֲ���----------------
	HR(mDirectInput->CreateDevice(GUID_SysKeyboard, &mDirectInputKeyboard, NULL));

	//���ü����豸�ӿڵ����ݸ�ʽ(ʹ��ϵͳԤ����ĸ�ʽ)
	HR(mDirectInputKeyboard->SetDataFormat(&c_dfDIKeyboard));

	//���ü����豸�ӿڵĺ����ȼ�,�������벻�������������
	HR(mDirectInputKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE));

	//����������ú��ˣ���ȡ���̿���ʹ��Ȩ
//	HR(mDirectInputKeyboard->Acquire());

	//---------------����, ��DirectInput�ӿ�������DirectInputDevice�ӿ� DirectInputMouse,�������úø��ֲ���---------------------

	HR(mDirectInput->CreateDevice(GUID_SysMouse, &mDirectInputMouse, NULL));

	//��������豸�ӿڵ����ݸ�ʽ(ʹ��ϵͳԤ����ĸ�ʽ)
	HR(mDirectInputMouse->SetDataFormat(&c_dfDIMouse));

	//��������豸�ӿڵĺ����ȼ�,������벻�������������
	HR(mDirectInputMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE));

	//���������ú��ˣ���ȡ������ʹ��Ȩ
	//HR(mDirectInputMouse->Acquire());

	return TRUE;

}


void Input::ShutDown()
{
	//�ͷ�DirectInputMouse
	ReleaseCOM(mDirectInputMouse);

	//�ͷ�mDirectInputKeyboard
	ReleaseCOM(mDirectInputKeyboard);

	//�ͷ�DirectInput
	ReleaseCOM(mDirectInput);
}


bool Input::Frame()
{
	bool result;

	//��ȡ���̵�����״̬
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	//��ȡ��������״̬
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	//�����ڼ����Ǻ����ĸı�
	ProcessInput();
	return true;
}



bool Input::ReadKeyboard()
{
	HRESULT result;
	//��ȡ����״̬
	result = mDirectInputKeyboard->GetDeviceState(sizeof(mKeyboardState), (LPVOID)&mKeyboardState);

	if (FAILED(result))
	{
		//����Ǽ���ʧȥ���е�����޷���ȡ����Ȩ,�����»�ȡ����Ȩ
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))  //���߲�����
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

	//��ȡ���״̬
	result = mDirectInputMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mMouseState);
   
	if (FAILED(result))
	{
		//��������ʧȥ���е�����޷���ȡ����Ȩ,�����»�ȡ����Ȩ
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))//���߲�����
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
	//��������ƫ����
	mMousePosXOffset = mMouseState.lX;
	mMousePosYOffset = mMouseState.lY;

	//��������λ��(ÿ֡�����и���)
	mMousePosX += mMouseState.lX;
	mMousePosY += mMouseState.lY;

   //ȷ���������Ļ֮��
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

//��ν�İ��¾���ǰһ�δ���up״̬����һ����down״̬
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
	//����Ҽ�����
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