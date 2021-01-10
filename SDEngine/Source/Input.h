#pragma once
#ifndef _INPUT_CLASS_H
#define _INPUT_CLASS_H

#include "Common/Macro.h"
//����ʹ�õ�D3DInput�İ汾
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>
#pragma comment(lib,"Dinput8.lib")
class Input
{
private:
	//DirectInput�ӿ�,IDirectInputDevice8�ӿ�
	IDirectInput8* mDirectInput;
	IDirectInputDevice8* mDirectInputKeyboard;
	IDirectInputDevice8* mDirectInputMouse;

	//��Ļ��Ⱥ͸߶�
	int mScreenWidth, mScreenHeight;

    //���̺�����ִ�״̬
	unsigned char mKeyboardState[256];
	DIMOUSESTATE mMouseState;

	//���λ��(win32����ϵ�µ�)
	int mMousePosX, mMousePosY;
	int mMousePosXOffset, mMousePosYOffset;


private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();
	bool Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight);

public:
	Input(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight);
	Input(const Input&);
	~Input();


public:
	
	void ShutDown();
	bool Frame();

	//ESC���Ƿ���
	bool IsEscapePressed();

	//W���Ƿ���
	bool IsWPressed();

	//S���Ƿ���
	bool IsSPressed();

	//A���Ƿ���
	bool IsAPressed();

	//D���Ƿ���
	bool IsDPressed();

	//A���Ƿ���
	bool IsQPressed();

	//D���Ƿ���
	bool IsEPressed();

	//����Ҽ��Ƿ���
	bool IsMouseRightButtuonPressed();

	bool IsKeyDown(int key);

	//��ȡ����λ��
	void GetMousePosition(int& MouseX, int &MouseY);

	void GetMousePositionOffset(int& MouseXOffset, int &MouseYOffset);
};
#endif 
