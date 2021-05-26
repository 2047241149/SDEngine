#include "GerstnerWave.h"


GerstnerWave::GerstnerWave(int waveWidth, int waveHeight, float waveGridSize,
	const vector<GerstnerWaveParam>& vecGerstnerWaveParam,
	int mUVTile): Wave(waveWidth, waveHeight, waveGridSize, mUVTile),
	m_vecGerstnerWaveParam(vecGerstnerWaveParam)
{

}


GerstnerWave::GerstnerWave(const GerstnerWave& other) :Wave(other)
{

}


GerstnerWave::~GerstnerWave()
{

}


//计算位置
void GerstnerWave::CalculateVertexPos(float time)
{
	UINT index = 0;

	//从左到右，从下到上
	for (int posZ = -mWaveHeight / 2; posZ <= mWaveHeight / 2; ++posZ)
	{
		for (int posX = -mWaveWidth / 2; posX <= mWaveWidth / 2; ++posX)
		{
			float fPositionX = (float)posX * mWaveGridSize;
			float fPositionZ = (float)posZ * mWaveGridSize;

			float fGerstnerX = fPositionX + GetPosXOffset(fPositionX, fPositionZ, time);
			float fGerstnerZ = fPositionZ + GetPosZOffset(fPositionX, fPositionZ, time);
			//pos
			mWaveVertexData[index].pos.x = fGerstnerX;
			mWaveVertexData[index].pos.z = fGerstnerZ;
			mWaveVertexData[index].pos.y = (float)GetWaveVertexHeight((float)fPositionX, (float)fPositionZ, time);
			++index;
		}
	}
}

void GerstnerWave::UpdateWaveData(float time)
{
	Wave::UpdateWaveData(time);
}

float GerstnerWave::GetWaveVertexHeight(int x, int z, float time)
{
	float fHeight = 0.0f;

	for (UINT iParamIndex = 0; iParamIndex < m_vecGerstnerWaveParam.size(); ++iParamIndex)
	{

		DirectionSineWaveParam& dirWaveParam = m_vecGerstnerWaveParam[iParamIndex];

		float fDdotXZ = dirWaveParam.fDrection.x * x + dirWaveParam.fDrection.y * z;

		//频率
		float w = 2.0f * XM_PI / dirWaveParam.fWaveLength;

		//相位差常量
		float phaseConstant = dirWaveParam.fSpeed * w;


		//posZ
		fHeight += (float)dirWaveParam.fAmplitude * sin(fDdotXZ * w + time * phaseConstant);

	}

	return fHeight;
}

float GerstnerWave::GetPosXOffset(float x, float z, float time)
{
	float fPosXOffset = 0.0f;

	for (UINT iParamIndex = 0; iParamIndex < m_vecGerstnerWaveParam.size(); ++iParamIndex)
	{
		GerstnerWaveParam& gerstnerWaveParam = m_vecGerstnerWaveParam[iParamIndex];

		float fDdotXZ = gerstnerWaveParam.fDrection.x * x + gerstnerWaveParam.fDrection.y * z;

		//频率
		float w = 2.0f * XM_PI / gerstnerWaveParam.fWaveLength;

		//相位差常量
		float phaseConstant = gerstnerWaveParam.fSpeed * w;

		fPosXOffset += (float)gerstnerWaveParam.fAmplitude * gerstnerWaveParam.fSteepness *
			gerstnerWaveParam.fDrection.x * cos(fDdotXZ * w + time * phaseConstant);
	}

	return fPosXOffset;
}


float GerstnerWave::GetPosZOffset(float x, float z, float time)
{
	float fPosZOffset = 0.0f;

	for (UINT iParamIndex = 0; iParamIndex < m_vecGerstnerWaveParam.size(); ++iParamIndex)
	{
		GerstnerWaveParam& gerstnerWaveParam = m_vecGerstnerWaveParam[iParamIndex];

		float fDdotXZ = gerstnerWaveParam.fDrection.x * x + gerstnerWaveParam.fDrection.y * z;

		//频率
		float w = 2.0f * XM_PI / gerstnerWaveParam.fWaveLength;

		//相位差常量
		float phaseConstant = gerstnerWaveParam.fSpeed * w;

		fPosZOffset += (float)gerstnerWaveParam.fAmplitude * gerstnerWaveParam.fSteepness *
			gerstnerWaveParam.fDrection.y * cos(fDdotXZ * w + time * phaseConstant);
	}

	return fPosZOffset;
}