#pragma once
#ifndef _GRAPHICS_SYSTEM_H
#define _GRAPHICS_SYSTEM_H

class GameObject;
class CubeCamera;
class SceneManager;
class SceneRender;

#include "Common/DirectxCore.h"
#include "Common/Macro.h"
#include"Light/Light.h"
#include"FPS.h"
#include<memory>
#include "Common/GraphicsConfig.h"
#include "GameObject/Mesh.h"

class GraphicsSystem
{

private:
	shared_ptr<SceneRender> sceneRender;

private:
	bool Init();

private:
	void InitDebugConsole();
	void CloseDebugConsole();

public:
	GraphicsSystem();
	GraphicsSystem(const GraphicsSystem&);
	~GraphicsSystem();

public:
	bool Tick(float deltaTime);
	void Render();
};
#endif // !_GRAPHICS_CLASS_H
