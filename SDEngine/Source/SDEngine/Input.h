#pragma once
#ifndef _INPUT_CLASS_H
#define _INPUT_CLASS_H

#include "Common/CoreMini.h"
#include "Event/EventBase.h"
//direct input version
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>

class Input
{
protected:
	//interface for direct input
	IDirectInput8* directInput;
	IDirectInputDevice8* directInputKeyboard;
	IDirectInputDevice8* directInputMouse;

    //keyboard state
	unsigned char keyboardState[256];
	DIMOUSESTATE mouseState;

	//mouse state
	int mousePosX, mousePosY;
	int mousePosXOffset, mousePosYOffset;

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

	//bool IsMouseButtuonPressed(MouseKey keyCode);
	bool IsMouseRightButtuonPressed();
	bool IsKeyDown(int key);
	void GetMousePosition(int& MouseX, int &MouseY);
	void GetMousePositionOffset(int& MouseXOffset, int &MouseYOffset);
};

#define GInput Input::Get()
#endif 
