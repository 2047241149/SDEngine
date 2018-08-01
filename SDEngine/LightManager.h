#pragma once
#ifndef _LIGHT_MANAGER_H
#define _LIGHT_MANAGER_H

#include "Light.h"
#include "DirectionLight.h"
#include "PointLight.h"
class LightManager
{

public:
	LightManager();
	LightManager(const LightManager&other);
	~LightManager();

	static shared_ptr<LightManager> Get();
public:
	void Add(shared_ptr<PointLight> spPointLight);
	void Add(shared_ptr<DirectionLight> spDirLight);
	shared_ptr<DirectionLight> GetMainLight();

public:
	vector<shared_ptr<PointLight>> m_vecPointLight;
	vector<shared_ptr<DirectionLight>> m_vecDirLight;

private:
	static shared_ptr<LightManager> m_pLightManager;

};
#endif // 


#define GLightManager (LightManager::Get())