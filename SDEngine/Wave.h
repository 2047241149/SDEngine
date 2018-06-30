#pragma once
#ifndef  _WAVE_H
#define _WAVE_H
#include "D3DClass.h"
#include "CommomVertexFormat.h"

struct WaveParam
{
public:
	float fAmplitude;
	float fSpeed;
	float fWaveLength;

	WaveParam::WaveParam(float amplitude, float speed, float waveLength):
		fAmplitude(amplitude), fSpeed(speed), fWaveLength(waveLength)
	{

	}
};

struct DirectionSineWaveParam :public WaveParam
{
public:
	XMFLOAT2 fDrection;

	DirectionSineWaveParam::DirectionSineWaveParam(XMFLOAT2 direction,
		float amplitude, float speed, float waveLength):
		WaveParam(amplitude, speed, waveLength), fDrection(direction)
	{

	}
};

struct CircleSineWaveParam :public WaveParam
{
public:
	XMFLOAT2 fCenter;

	CircleSineWaveParam::CircleSineWaveParam(XMFLOAT2 center,
		float amplitude, float speed, float waveLength) :
		WaveParam(amplitude,speed,waveLength),fCenter(center)
	{

	}
};


struct GerstnerWaveParam : public DirectionSineWaveParam
{
public:
	float fSteepness;

	GerstnerWaveParam(XMFLOAT2 direction, float amplitude, float speed,
		float waveLength, float steepness) :
		DirectionSineWaveParam(direction, amplitude, speed, waveLength),
		fSteepness(steepness)
	{

	}
};

class Wave
{
protected:
	vector<VertexPCNTT> mWaveVertexData;
	vector<WORD> mWaveIndexData;

	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	
	//UV的Tile长度，代表每mUVTile格子占据1.0的UV值
	int mUVTile;

	//构建WaveGrid的参数
	int mWaveWidth;
	int mWaveHeight;
	float mWaveGridSize;

public:
	//位置
	XMFLOAT3 mPosition;

private:

	void Init();
	XMFLOAT3 CrossNormal(const XMFLOAT3& vertex1, const XMFLOAT3& vertex2, const XMFLOAT3& vertex3);
	void NormalizeFloat3(XMFLOAT3& normal);
	float Dot(const XMFLOAT3& vec1, const XMFLOAT3& vec2);

protected:

	virtual float GetWaveVertexHeight(int x, int z, float time) = 0;

	//计算位置（波的计算位置方式可能不一样）
	virtual void CalculateVertexPos(float time);

	//计算切线
	void CalculateVertexTangent();

	//计算UV
	void CalculateVertexUV();

	//计算法线
	void CalculateVertexNormal();
		

public:
	
	virtual void UpdateWaveData(float time);
	void Render();

	Wave(int waveWidth, int waveHeight,float waveGridSize,int mUVTile = 20);

	Wave(int waveWidth, int waveHeight, float waveGridSize);

	Wave(const Wave& other);
	~Wave();

	void SetWaveParam(int uvTile);

};
#endif // ! _WAVE_H
