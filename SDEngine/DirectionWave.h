#pragma once
#ifndef _DIRECTION_WAVE_H
#define _DIRECTION_WAVE_H
#include "Wave.h"

class DirectionWave : public Wave
{

private:
	vector<DirectionSineWaveParam> m_vecDirWaveParam;

protected:
	//º∆À„Œª÷√
	virtual void CalculateVertexPos(float time) override;
	virtual float GetWaveVertexHeight(int x, int z, float time) override;

public:
	DirectionWave(int waveWidth, int waveHeight, float waveGridSize,
		const vector<DirectionSineWaveParam>& vecDirWaveParam,
		 int mUVTile = 20);

	DirectionWave(const DirectionWave& other);
	~DirectionWave();

	virtual void UpdateWaveData(float time) override;
};

#endif
