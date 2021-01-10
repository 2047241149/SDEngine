#pragma once
#ifndef _CIRCLE_WAVE_H
#define _CIRCLE_WAVE_H
#include "Wave.h"

class CircleWave:public Wave
{

private:
	vector<CircleSineWaveParam> m_vecCircleWaveParam;

protected:
	//º∆À„Œª÷√
	virtual void CalculateVertexPos(float time) override;
	virtual float GetWaveVertexHeight(int x, int z, float time) override;

public:
	CircleWave(int waveWidth, int waveHeight, float waveGridSize,
		const vector<CircleSineWaveParam>& vecCircleWaveParam,int mUVTile = 20);
	CircleWave(const CircleWave& other);
	~CircleWave();

	virtual void UpdateWaveData(float time) override;

};
#endif // !_CIRCLE_WAVE_H