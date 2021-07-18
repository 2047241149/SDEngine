#pragma once
#ifndef _FPS_H
#define _FPS_H

#include<Windows.h>
#include<mmsystem.h>
#include<iostream>
#include<memory>
using namespace std;

class FPS
{
private:
	int mFPS;  //每秒运行的帧数
	int mCount; //运来计算每秒帧数的计数器
	unsigned long mStartTime;  
	unsigned long m_fFistStartTime;

private:
	static shared_ptr<FPS> single;

private:
	void Initialize();

public:
	FPS();
	~FPS();

	static shared_ptr<FPS> Get();

public:
	void Frame();
	int GetFPS();
	float GetDeltaTime();
	float GetTime();
};

#define GFPS FPS::Get()
#endif // !_FPS_CLASS_H
