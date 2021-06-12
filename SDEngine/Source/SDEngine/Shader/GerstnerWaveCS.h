#pragma once
#ifndef _GERSTNER_WAVE_CS_H
#define _GERSTNER_WAVE_CS_H


#include "CoreMinimal.h"

struct CBGerstnerWaveUpdate
{
	float fCurrentTime;
	XMFLOAT3 pad;
};

//严格对应ComputerShader的ConstantBuffer
struct GerstnerParam
{
public:
	float m_fAmplitude;
	float m_fSpeed;
	float m_fWaveLength;
	float m_fsteepness;
	XMFLOAT2 m_direction;
	XMFLOAT2 m_pad;


	GerstnerParam::GerstnerParam(XMFLOAT2 direction, float fAmplitude, float fSpeed, float fWaveLength, float fSteepnees):
		m_direction(direction),
		m_fAmplitude(fAmplitude),
		m_fSpeed(fSpeed),
		m_fWaveLength(fWaveLength),
		m_fsteepness(fSteepnees),
		m_pad(XMFLOAT2(0.0f,0.0f))
	{

	}

	GerstnerParam::GerstnerParam()
	{

	}
};

struct CBGerstnerWaveNoUpdate
{
	GerstnerParam gerstnerData[3];
	float fWaveGridSize;
	float fGroundCountX; //X方向有几个线程团   
	float fGroundCountY; //Y方向有几个线程团
	float fUVTile;
	float fUVMoveSpeed;
	XMFLOAT3 pad;
};

class GerstnerWaveCS
{
public:
	GerstnerWaveCS(UINT iWaveWidth,UINT iWaveHeight, WCHAR* csWavePosPath, WCHAR* csWaveNormalPath, const CBGerstnerWaveNoUpdate& stWaveNoUpdate);
	GerstnerWaveCS(const GerstnerWaveCS& other);
	~GerstnerWaveCS();
	void UpdateWaveCB(float  fCurrentTime);
	void Render();
	XMMATRIX GetWorldMatrix();

private:
	void Initlize(WCHAR* csWavePosPath, WCHAR* csWaveNormalPath);
	void CreateComputerShader(WCHAR* csWavePosPath, WCHAR* csWaveNormalPath);
	void CreateConstantBuffer();
	void CreateStructBuffer();
	void UpdateConstanBuffer();
	void CreateBufferUAV();
	void CreateAndCopyToDebugBuf();
	void InitializeIndexBuffer();
	void CalculateWaveData();

private:
	ID3D11ComputeShader* m_pWavePosCS;
	ID3D11ComputeShader* m_pWaveNormalCS;
	ID3D11UnorderedAccessView* m_pWaveDataUAV;
	ID3D11Buffer* m_pCBWaveUpdate;
	ID3D11Buffer* m_pCBWaveNoUpdate;
	ID3D11Buffer* m_pWaveIndexBuffer;
	ID3D11Buffer* m_pWaveVertexBuffer;
	ID3D11Buffer* m_pWaveDataBuffer;
	UINT m_nWaveWidth;
	UINT m_nWaveHeight;
	XMFLOAT3 m_Position;
	vector<WORD> m_vecWaveIndexData;
	vector<VertexPCNTT> m_vecWaveVertexData;
	CBGerstnerWaveNoUpdate m_stWaveNoUpdate;
};
#endif // !_GERSTNER_WAVE_H
