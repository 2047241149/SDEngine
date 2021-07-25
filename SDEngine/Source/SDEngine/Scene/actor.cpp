#include "Actor.h"

Actor::Actor():
	scene(nullptr)
{
}

Actor::Actor(entt::entity inHandle, Scene* inScene) :
	handle(inHandle),
	scene(inScene)
{
}