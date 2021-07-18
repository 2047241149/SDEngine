#pragma once
#ifndef _SCENE_RENDER_H
#define _SCENE_RENDER_H


class SceneRender
{
public:
	SceneRender();
	~SceneRender();

public:
	virtual bool Init() { return true; };
	virtual void PreRender() = 0;
	virtual void Tick(float deltaTime) = 0;
	virtual void Render() = 0;
};

#endif // !_SCENE_RENDER_H
