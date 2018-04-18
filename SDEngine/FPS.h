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

private:
	static shared_ptr<FPS> mInstance;

private:
	void Initialize();


public:
	FPS();
	FPS(const FPS&);
	~FPS();

	static FPS* GetInstance();

public:

	void Frame();
	int GetFPS();
	float GetDeltaTime();
	float GetTime();
};
#endif // !_FPS_CLASS_H
