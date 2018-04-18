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
	int mFPS;  //ÿ�����е�֡��
	int mCount; //��������ÿ��֡���ļ�����
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
