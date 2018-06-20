#include "CircleWave.h"

CircleWave::CircleWave(int waveWidth, int waveHeight, float waveGridSize,
	XMFLOAT2 circleCenter,float amplitude, float speed, float waveLength,
	 int mUVTile):Wave(waveWidth, waveHeight, waveGridSize,amplitude, speed, waveLength,mUVTile),
	mCircleCenter(circleCenter)
{

}


CircleWave::CircleWave(const CircleWave& other) :Wave(other)
{

}


CircleWave::~CircleWave()
{

}


//计算位置
void CircleWave::CalculateVertexPos(float time)
{
	Wave::CalculateVertexPos(time);
}

void CircleWave::UpdateWaveData(float time)
{
	Wave::UpdateWaveData(time);
}

float CircleWave::GetWaveVertexHeight(int x, int z, float time)
{
	float offsetX = mCircleCenter.x - x;
	float offsetZ = mCircleCenter.y - z;
	float distanceToCenter = sqrt(offsetX * offsetX + offsetZ *offsetZ);
	offsetX /= distanceToCenter;
	offsetZ /= distanceToCenter;
	float DdotXZ = offsetX * x + offsetZ * z;


	//频率
	float w = 2.0f * XM_PI / mWaveLength;

	//相位差常量
	float phaseConstant = mSpeed * w;

	//posZ
	float fWeightHeight = (float)mAmplitude * sin(DdotXZ * w + time * phaseConstant);

	return fWeightHeight;
}