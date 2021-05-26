#include"FPS.h"

FPS::FPS()
{
	Initialize();
}


FPS::FPS(const FPS&other)
{

}


FPS::~FPS()
{

}

void FPS::Initialize()
{
	mFPS = 0;
	mCount = 0;
	mStartTime = 0;
	m_fFistStartTime = timeGetTime();
}


void FPS::Frame()
{
	++mCount;  
	float currentTime = timeGetTime();

	if (currentTime >= (mStartTime + 1000))
	{
		mFPS = mCount;
		mCount = 0;
		mStartTime = currentTime;
	}
}

float FPS::GetDeltaTime()
{
	if (mFPS >= 1)
	{
		return 1.0f / float(mFPS);
	}
	else
	{
		return 1.0f;
	}
}

shared_ptr<FPS> FPS::Get()
{
	if (single == nullptr)
	{
		single = shared_ptr<FPS>(new FPS());
	}
	return single;
}

float FPS::GetTime()
{
	return (float)(timeGetTime() - m_fFistStartTime)/ 1000.0f;
}

int FPS::GetFPS()
{
	return mFPS;
}

shared_ptr<FPS> FPS::single = nullptr;