#include "DirectionWave.h"

DirectionWave::DirectionWave(int waveWidth, int waveHeight, float waveGridSize,
	const vector<DirectionSineWaveParam>& vecDirWaveParam,
	int mUVTile):Wave(waveWidth, waveHeight, waveGridSize,mUVTile),m_vecDirWaveParam(vecDirWaveParam)
{

}


DirectionWave::DirectionWave(const DirectionWave& other):Wave(other)
{

}


DirectionWave::~DirectionWave()
{

}


//计算位置
void DirectionWave::CalculateVertexPos(float time)
{
	Wave::CalculateVertexPos(time);
}

void DirectionWave::UpdateWaveData(float time)
{
	Wave::UpdateWaveData(time);
}

float DirectionWave::GetWaveVertexHeight(int x, int z, float time)
{
	float fHeight = 0.0f;

	for (UINT iParamIndex = 0; iParamIndex < m_vecDirWaveParam.size(); ++iParamIndex)
	{

		DirectionSineWaveParam& dirWaveParam = m_vecDirWaveParam[iParamIndex];

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