#pragma once
#include "CoreMinimal.h"
#include "Event/Event.h"

class SD_API Layer
{
protected:
	string layerName;

public:
	Layer(const string& name = "layer"):
		layerName(name)
	{
	};

	virtual ~Layer() = default;

public:
	virtual void OnAttach() {};
	virtual void OnDetach() {};
	virtual void OnTick(float deltaTime) {};
	virtual void OnEvent(Event& event) {};
	virtual void OnImguiRender() {};
	virtual void End() {};

public:
	string GetLayerName() { return layerName; }
};