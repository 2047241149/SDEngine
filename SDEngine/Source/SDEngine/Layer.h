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
	virtual void OnUpdate(float deltaTime) {};
	virtual void OnEvent(Event& event) {};

public:
	string GetLayerName() { return layerName; }

};