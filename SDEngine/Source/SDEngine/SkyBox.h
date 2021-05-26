#pragma once
#ifndef _SKY_BOX_H
#define _SKY_BOX_H

#include "Common/CoreMini.h"
class Texture;
class GameObject;
class GeometryBuffer;

class SkyBox
{
public:
	SkyBox(WCHAR* cubeMapFileName);
	SkyBox(ID3D11ShaderResourceView* inSrv);
	SkyBox(const SkyBox& other);
	~SkyBox();

public:
	void Render(GeometryBuffer* geometryBuffer);
	void SetTexture(ID3D11ShaderResourceView* inSrv);

private:
	bool Init(WCHAR* cubeMapFileName);
	void ShutDown();

private:
	shared_ptr<Texture> skyBoxTexture;
	shared_ptr<GameObject> skyBoxGameObject;
	ID3D11ShaderResourceView* cubeSrv = nullptr;
};
#endif // !_SKY_BOX_H