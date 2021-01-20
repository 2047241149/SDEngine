#pragma once
#ifndef _IRRADIANCE_CUBE_MAP_H
#define _IRRADIANCE_CUBE_MAP_H

#include "Common/CoreMini.h"
class Texture;
class GameObject;
class GeometryBuffer;
class HdrTexture;
class CubeCamera;
class RenderCubeMap;


class IrradianceCubeMap
{
public:
	IrradianceCubeMap(char* hdrEquirectangularMapFile);
	IrradianceCubeMap(const IrradianceCubeMap& other);
	~IrradianceCubeMap();

public:
	void Render();
	ID3D11ShaderResourceView* GetSrv();
	ID3D11ShaderResourceView* GetTexture2DSrv();

private:
	bool Init(char* cubeMapFileName);
	void ShutDown();

private:
	shared_ptr<HdrTexture> hdrTetxure;
	shared_ptr<GameObject> cubeGameObject;
	shared_ptr<RenderCubeMap> renderCubeMap;
	shared_ptr<CubeCamera> cubeCamera;

};
#endif // !_SKY_BOX_H