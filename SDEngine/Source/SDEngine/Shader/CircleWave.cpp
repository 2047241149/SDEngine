#include "CircleWave.h"

CircleWave::CircleWave(int waveWidth, int waveHeight, float waveGridSize,
	const vector<CircleSineWaveParam>& vecCircleWaveParam,
	 int uvTile):Wave(waveWidth, waveHeight, waveGridSize, uvTile),
	m_vecCircleWaveParam(vecCircleWaveParam)
{

}


CircleWave::CircleWave(const CircleWave& other) :Wave(other)
{

}


CircleWave::~CircleWave()
{

}


//����λ��
void CircleWave::CalculateVertexPos(float time)
{
	Wave::CalculateVertexPos(time);
}


void CircleWave::UpdateWaveData(float time)
{
	Wave::UpdateWaveData(time);
}


//��Q���ļ������
float CircleWave::GetWaveVertexHeight(int x, int z, float time)
{
	float fHeight = 0.0f;

	for (UINT iParamIndex = 0; iParamIndex < m_vecCircleWaveParam.size(); ++iParamIndex)
	{
		CircleSineWaveParam& circleWaveParam = m_vecCircleWaveParam[iParamIndex];
		float fOffsetX = circleWaveParam.fCenter.x - x;
		float fOffsetZ = circleWaveParam.fCenter.y - z;

		//+0.000001��Ϊ�˱�֤��ĸ��Ϊ0
		float distanceToCenter = sqrt(fOffsetX * fOffsetX + fOffsetZ *fOffsetZ) + 0.000001f;
		fOffsetX /= distanceToCenter;
		fOffsetZ /= distanceToCenter;
		float DdotXZ = fOffsetX * x + fOffsetZ * z;

		//Ƶ��
		float w = 2.0f * XM_PI / circleWaveParam.fWaveLength;

		//��λ���
		float phaseConstant = circleWaveParam.fSpeed * w;
		float sinParams = sin(DdotXZ * w + time * phaseConstant);

		//posZ
		fHeight += (float)circleWaveParam.fAmplitude * sinParams;
	}

	return fHeight;
}