#pragma once
#ifndef _GERSTNER_WAVE_H
#define _GERSTNER_WAVE_H
#include "Wave.h"


class GerstnerWave : public Wave
{

private:
	vector<GerstnerWaveParam> m_vecGerstnerWaveParam;


private:
	float GetPosXOffset(float x, float z, float time);
	float GetPosZOffset(float x, float z, float time);

protected:
	//º∆À„Œª÷√
	virtual void CalculateVertexPos(float time) override;
	virtual float GetWaveVertexHeight(int x, int z, float time) override;

public:
	GerstnerWave(int waveWidth, int waveHeight, float waveGridSize,
		const vector<GerstnerWaveParam>& vecGerstnerWaveParam,
		int mUVTile = 20);

	GerstnerWave(const GerstnerWave& other);
	~GerstnerWave();

	virtual void UpdateWaveData(float time) override;
};
#endif // !_GERSTER_WAVE_H
