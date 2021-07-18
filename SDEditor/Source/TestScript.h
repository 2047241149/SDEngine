#pragma once
#include "SDEngine/Scene/ScriptableLogic.h"
#include "SDEngine/Scene/TransformComponent.h"
#include "SDEngine/Log.h"

class TestScript : public ScriptableLogic
{
public:
	void BeginPlay() override
	{
		Log::Info("TestScript BeginPlay");
	}

	void End() override
	{
		Log::Info("TestScript End");
	}

	void Tick(float deltaTime) override
	{
		//Log::Info("TestScript Tick{0}", deltaTime);
	}
};