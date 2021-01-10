#include "GameObjectManager.h"

GameObjectManager::GameObjectManager()
{

}

GameObjectManager::GameObjectManager(const GameObjectManager& other)
{

}

GameObjectManager::~GameObjectManager()
{

}

shared_ptr<GameObjectManager> GameObjectManager::Get()
{
	if (nullptr == m_pGameObjectManager)
	{
		m_pGameObjectManager = shared_ptr<GameObjectManager>(new GameObjectManager());
	}

	return m_pGameObjectManager;
}

void GameObjectManager::Add(shared_ptr<GameObject> pGameObject)
{
	m_vecGameObject.push_back(pGameObject);
}

shared_ptr<GameObjectManager> GameObjectManager::m_pGameObjectManager = nullptr;