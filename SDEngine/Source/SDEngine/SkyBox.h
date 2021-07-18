#pragma once
#ifndef _SKY_BOX_H
#define _SKY_BOX_H

#include "CoreMinimal.h"
class Texture;
class GameObject;
class GeometryBuffer;

class SkyBox
{
public:
	SkyBox(const string& file);
	SkyBox(ID3D11ShaderResourceView* inSrv);
	~SkyBox();

public:
	void Render(GeometryBuffer* geometryBuffer);
	void SetTexture(ID3D11ShaderResourceView* inSrv);

private:
	bool Init(const string& file);
	void ShutDown();

private:
	shared_ptr<Texture> skyBoxTexture;
	shared_ptr<GameObject> skyBoxGameObject;
	ID3D11ShaderResourceView* cubeSrv = nullptr;
};
#endif