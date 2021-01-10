#pragma once
#ifndef _GAME_OBJECT_MANAGER_H
#define _GAME_OBJECT_MANAGER_H

#include "GameObject.h"

class GameObjectManager
{
public:
	GameObjectManager();
	GameObjectManager(const GameObjectManager& other);
	~GameObjectManager();

public:
	static shared_ptr<GameObjectManager> Get();
	void Add(shared_ptr<GameObject> pGameObject);
private:
	static shared_ptr<GameObjectManager> m_pGameObjectManager;
	
public:
	vector<shared_ptr<GameObject>> m_vecGameObject;


#define GGameObjectManager (GameObjectManager::Get())
};
#endif