#pragma once
#include "ScriptableLogic.h"
#include <iostream>
#include "SDEngine/Log.h"
#include <string>
using namespace std;

class ScriptComponent
{
public:
	ScriptComponent(string name = "logic"):
		scriptName(name),
		instance(nullptr),
		bCanTick(true)
	{
		instance = nullptr;
	}

public:
	string scriptName;
	ScriptableLogic* instance;
	function<ScriptableLogic*()> CreateInstanceFunc;
	bool bCanTick;

	void BeginPlay()
	{
		instance->BeginPlay();
	}

	void End()
	{
		instance->End();
	}

	void Tick(float deltaTime)
	{
		instance->Tick(deltaTime);
	}

	template<typename T>
	void Bind()
	{
		CreateInstanceFunc = []() { return new T(); };
	}	
};