#pragma once
#ifndef _INPUT_CLASS_H
#define _INPUT_CLASS_H

#include "Common/CoreMini.h"
//定义使用的D3DInput的版本
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>
#pragma comment(lib,"Dinput8.lib")
class Input
{
private:
	//DirectInput接口,IDirectInputDevice8接口
	IDirectInput8* mDirectInput;
	IDirectInputDevice8* mDirectInputKeyboard;
	IDirectInputDevice8* mDirectInputMouse;

	//屏幕宽度和高度
	int mScreenWidth, mScreenHeight;

    //键盘和鼠标现存状态
	unsigned char mKeyboardState[256];
	DIMOUSESTATE mMouseState;

	//鼠标位置(win32坐标系下的)
	int mMousePosX, mMousePosY;
	int mMousePosXOffset, mMousePosYOffset;

private:
	static shared_ptr<Input> single;

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

public:
	Input();
	Input(const Input&);
	~Input();

	static shared_ptr<Input> Get();

public:
	bool Init();

	void ShutDown();

	bool Tick();

	//ESC键是否按下
	bool IsEscapePressed();

	//W键是否按下
	bool IsWPressed();

	//S键是否按下
	bool IsSPressed();

	//A键是否按下
	bool IsAPressed();

	//D键是否按下
	bool IsDPressed();

	//A键是否按下
	bool IsQPressed();

	//D键是否按下
	bool IsEPressed();

	//鼠标右键是否按下
	bool IsMouseRightButtuonPressed();

	bool IsKeyDown(int key);

	//获取鼠标的位置
	void GetMousePosition(int& MouseX, int &MouseY);

	void GetMousePositionOffset(int& MouseXOffset, int &MouseYOffset);
};

#define GInput Input::Get()
#endif 
