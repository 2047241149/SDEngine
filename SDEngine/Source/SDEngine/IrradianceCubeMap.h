#pragma once
#ifndef _IRRADIANCE_CUBE_MAP_H
#define _IRRADIANCE_CUBE_MAP_H

#include "CoreMinimal.h"
class Texture;
class GameObject;
class GeometryBuffer;
class CubeCamera;
class RenderCubeMap;


class IrradianceCubeMap
{
public:
	IrradianceCubeMap(const string& file, int textureWidth = 64, int textureHeight = 64);
	~IrradianceCubeMap();

public:
	void Render();
	ID3D11ShaderResourceView* GetIrradianceSrv();
	ID3D11ShaderResourceView* GetCubeMapSrv();

private:
	bool Init(const string& file, int textureWidth = 64, int textureHeight = 64);
	void ShutDown();

private:
	shared_ptr<Texture> hdrCubeMap;
	shared_ptr<GameObject> cubeGameObject;
	shared_ptr<RenderCubeMap> renderCubeMap;
	shared_ptr<CubeCamera> cubeCamera;

};
#endif