#pragma once
#ifndef _PREFILTER_CUBE_MAP_H
#define _PREFILTER_CUBE_MAP_H

#include "CoreMinimal.h"
class Texture;
class GameObject;
class GeometryBuffer;
class CubeCamera;


class PrefliterCubeMap
{
public:
	PrefliterCubeMap(WCHAR* hdrCubeMapFile, int textureWidth = 64, int textureHeight = 64);
	PrefliterCubeMap(const PrefliterCubeMap& other);
	~PrefliterCubeMap();

public:
	bool Render();
	ID3D11ShaderResourceView* GetPrefilterCubeMapSrv();
	ID3D11ShaderResourceView* GetCubeMapSrv();

private:
	bool Init(WCHAR* cubeMapFileName, int textureWidth = 64, int textureHeight = 64);
	void ShutDown();

private:
	shared_ptr<Texture> hdrCubeMap;
	shared_ptr<GameObject> cubeGameObject;
	shared_ptr<CubeCamera> cubeCamera;
	int textureWidth, textureHeight;
	ID3D11Texture2D* cubeMapTexture;
	ID3D11ShaderResourceView* srv;
};
#endif