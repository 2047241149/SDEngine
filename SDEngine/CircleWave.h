#pragma once
#ifndef _CIRCLE_WAVE_H
#define _CIRCLE_WAVE_H
#include "Wave.h"

class CircleWave:public Wave
{

private:
	XMFLOAT2 mCircleCenter;

protected:
	//º∆À„Œª÷√
	virtual void CalculateVertexPos(float time) override;
	virtual float GetWaveVertexHeight(int x, int z, float time) override;

public:
	CircleWave(int waveWidth, int waveHeight, float waveGridSize,
		XMFLOAT2 circleCenter = XMFLOAT2(0.0f,0.0f),float amplitude = 4.0f, 
		float speed = 2.0f, float waveLength = 20.0f,int mUVTile = 20);
	CircleWave(const CircleWave& other);
	~CircleWave();

	virtual void UpdateWaveData(float time) override;

};
#endif // !_CIRCLE_WAVE_H
