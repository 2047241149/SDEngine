#pragma once
#ifndef _DIRECTION_WAVE_H
#define _DIRECTION_WAVE_H
#include "Wave.h"

class DirectionWave : public Wave
{

private:
	XMFLOAT2 mDirection;
protected:
	//º∆À„Œª÷√
	virtual void CalculateVertexPos(float time) override;
	virtual float GetWaveVertexHeight(int x, int z, float time) override;

public:
	DirectionWave(int waveWidth, int waveHeight, float waveGridSize,
		XMFLOAT2 direction = XMFLOAT2(1.0f, 0.0f),
		float amplitude = 4.0f, float speed = 2.0f, float waveLength = 20.0f,
		 int mUVTile = 20);

	DirectionWave(const DirectionWave& other);
	~DirectionWave();

	virtual void UpdateWaveData(float time) override;
};
#endif
