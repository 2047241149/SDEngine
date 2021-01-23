#pragma once
#ifndef _IRRADIANCE_CUBE_MAP_H
#define _IRRADIANCE_CUBE_MAP_H

#include "Common/CoreMini.h"
class Texture;
class GameObject;
class GeometryBuffer;
class CubeCamera;
class RenderCubeMap;


class IrradianceCubeMap
{
public:
	IrradianceCubeMap(WCHAR* hdrCubeMapFile, int textureWidth = 64, int textureHeight = 64);
	IrradianceCubeMap(const IrradianceCubeMap& other);
	~IrradianceCubeMap();

public:
	void Render();
	ID3D11ShaderResourceView* GetIrradianceSrv();
	ID3D11ShaderResourceView* GetCubeMapSrv();

private:
	bool Init(WCHAR* cubeMapFileName, int textureWidth = 64, int textureHeight = 64);
	void ShutDown();

private:
	shared_ptr<Texture> hdrCubeMap;
	shared_ptr<GameObject> cubeGameObject;
	shared_ptr<RenderCubeMap> renderCubeMap;
	shared_ptr<CubeCamera> cubeCamera;

};
#endif // !_SKY_BOX_H