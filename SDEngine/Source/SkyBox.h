#pragma once
#ifndef _SKY_BOX_H
#define _SKY_BOX_H
#include "CoreMini.h"
class Texture;
class GameObject;
class GeometryBuffer;

class SkyBox
{
public:
	SkyBox(WCHAR* cubeMapFileName);
	SkyBox(const SkyBox& other);
	~SkyBox();

public:
	void Render(GeometryBuffer* geometryBuffer);

private:
	bool Init(WCHAR* cubeMapFileName);
	void ShutDown();

private:
	shared_ptr<Texture> skyBoxTexture;
	shared_ptr<GameObject> skyBoxGameObject;
};
#endif // !_SKY_BOX_H
