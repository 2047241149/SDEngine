#pragma once
#ifndef _INPUT_CLASS_H
#define _INPUT_CLASS_H

#include "CoreMinimal.h"
#include "Event/KeyCode.h"

//direct input version
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>

class SD_API Input
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
	bool Init();

public:
	Input();
	Input(const Input&);
	~Input();

	static shared_ptr<Input> Get();

public:
	void ShutDown();
	static bool Tick();

	static bool IsMouseButtuonPressed(EMouse keyCode);
	static bool IsKeyDown(EKey keyCode);
	static void GetMousePosition(int& MouseX, int &MouseY);
	static void GetMousePositionOffset(int& MouseXOffset, int &MouseYOffset);
};

#endif 
