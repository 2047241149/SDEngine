#include "LightManager.h"

LightManager::LightManager()
{
	m_vecPointLight.clear();
	m_vecDirLight.clear();
}


LightManager::LightManager(const LightManager&other)
{

}


LightManager::~LightManager()
{

}


void LightManager::Add(shared_ptr<PointLight> spPointLight)
{
	m_vecPointLight.push_back(spPointLight);
}


void LightManager::Add(shared_ptr<DirectionLight> spDirLight)
{
	m_vecDirLight.push_back(spDirLight);
}

shared_ptr<DirectionLight> LightManager::GetMainLight()
{
	if (m_vecDirLight.size() > 0)
	{
		return m_vecDirLight[0];
	}
	else
	{
		return nullptr;
	}
}

void LightManager::GetPointLights(vector<PointLightParams>& lightArray)
{
	lightArray.empty();

	for (auto& it : m_vecPointLight)
	{
		PointLightParams light;
		light.lightColor = it->GetLightColor();
		light.lightPos = it->GetPosition();
		light.radius = it->GetRadius();
		light.attenuation = it->GetLightAttenuation();
		lightArray.push_back(light);
	}
}

shared_ptr<LightManager> LightManager::Get()
{
	if (nullptr == m_pLightManager)
	{
		m_pLightManager = shared_ptr<LightManager>(new LightManager());
	}

	return m_pLightManager;
}

shared_ptr<LightManager> LightManager::m_pLightManager = nullptr;