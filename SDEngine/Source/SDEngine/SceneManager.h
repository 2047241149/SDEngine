#pragma once
#ifndef _SCENE_MANAGER_H
#define _SCENE_MANAGER_H

#include "Scene/MeshComponent.h"
class GameObject;
class SkyBox;
class Input;

/*
** SceneManager Class, manage for mesh,light, skyBox and so on
*/
class SceneManager
{

public:
	//mesh object
	shared_ptr<GameObject> mOpacitySphereObject;
	shared_ptr<GameObject> mTransSphereObject;
	shared_ptr<GameObject> mHeadObject;
	shared_ptr<GameObject> mSponzaNoBottom;
	shared_ptr<GameObject> mSponzaBottom;
	shared_ptr<GameObject> m_pPointVolume;

	//sky_box
	shared_ptr<SkyBox> skyBox;

	bool bDebugLightCount = false;

private:
	static shared_ptr<SceneManager> single;

public:
	SceneManager();
	SceneManager(const SceneManager&);
	~SceneManager();
	static shared_ptr<SceneManager> Get();

public:
	bool Init();
	void Tick(float deltaTime); 
};

#define GSceneManager SceneManager::Get()
#endif // !_SCENE_MANAGER_H
