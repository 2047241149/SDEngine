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
	//timeGetTime·µ»ØµÄ
	if (timeGetTime() >= (mStartTime + 1000))
	{
		mFPS = mCount;
		mCount = 0;
		mStartTime = timeGetTime();
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

FPS* FPS::GetInstance()
{
	if (mInstance == nullptr)
	{
		mInstance = shared_ptr<FPS>(new FPS());
	}
	return mInstance.get();
}

float FPS::GetTime()
{
	return (float)(timeGetTime() - m_fFistStartTime)/ 1000.0f;
}


int FPS::GetFPS()
{
	return mFPS;
}
shared_ptr<FPS> FPS::mInstance = nullptr;