#pragma once
#ifndef  _WAVE_H
#define _WAVE_H
#include "D3DClass.h"
#include "CommomVertexFormat.h"

class Wave
{
protected:
	vector<VertexPCNTT> mWaveVertexData;
	vector<WORD> mWaveIndexData;

	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	//海浪参数
	float mAmplitude;
	float mSpeed;
	float mWaveLength;


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

	//计算位置（Wave的每个基类都重写此函数，因为每种波的计算位置方式都不一样）
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

	Wave(int waveWidth, int waveHeight,float waveGridSize,
		float amplitude = 4.0f, float speed = 2.0f, float waveLength = 20.0f,int mUVTile = 20);

	Wave(int waveWidth, int waveHeight, float waveGridSize);

	Wave(const Wave& other);
	~Wave();

	void SetWaveParam(float amplitude, float speed, float waveLength, XMFLOAT2 direction, int uvTile);

};
#endif // ! _WAVE_H
