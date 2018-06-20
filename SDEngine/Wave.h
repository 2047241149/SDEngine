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

	//���˲���
	float mAmplitude;
	float mSpeed;
	float mWaveLength;


	//UV��Tile���ȣ�����ÿmUVTile����ռ��1.0��UVֵ
	int mUVTile;

	//����WaveGrid�Ĳ���
	int mWaveWidth;
	int mWaveHeight;
	float mWaveGridSize;

public:
	//λ��
	XMFLOAT3 mPosition;

private:

	void Init();
	XMFLOAT3 CrossNormal(const XMFLOAT3& vertex1, const XMFLOAT3& vertex2, const XMFLOAT3& vertex3);
	void NormalizeFloat3(XMFLOAT3& normal);
	float Dot(const XMFLOAT3& vec1, const XMFLOAT3& vec2);

protected:

	virtual float GetWaveVertexHeight(int x, int z, float time) = 0;

	//����λ�ã�Wave��ÿ�����඼��д�˺�������Ϊÿ�ֲ��ļ���λ�÷�ʽ����һ����
	virtual void CalculateVertexPos(float time);

	//��������
	void CalculateVertexTangent();

	//����UV
	void CalculateVertexUV();

	//���㷨��
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
