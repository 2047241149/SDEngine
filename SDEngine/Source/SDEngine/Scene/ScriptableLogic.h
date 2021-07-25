#pragma once
#include "Actor.h"

class ScriptableLogic
{
public:
	ScriptableLogic() = default;

public:
	template<typename T>
	T& GetComponent()
	{
		return actor.GetComponent<T>();
	}

	virtual void BeginPlay() {};
	virtual void End() {};
	virtual void Tick(float detalTime) {};
	void SetActor(Actor inActor) { this->actor = inActor; };

protected:
	Actor actor;
};