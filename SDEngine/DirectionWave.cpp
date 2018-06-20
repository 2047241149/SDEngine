#include "DirectionWave.h"

DirectionWave::DirectionWave(int waveWidth, int waveHeight, float waveGridSize, XMFLOAT2 direction,
	float amplitude, float speed, float waveLength,
	 int mUVTile):Wave(waveWidth, waveHeight, waveGridSize,
		amplitude, speed,  waveLength,mUVTile),mDirection(direction)
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
	float DdotXZ = mDirection.x * x + mDirection.y *z;

	//频率
	float w = 2.0f * XM_PI / mWaveLength;

	//相位差常量
	float phaseConstant = mSpeed * w;


	//posZ
	float fWeightHeight = (float)mAmplitude * sin(DdotXZ * w + time * phaseConstant);

	return fWeightHeight;
}