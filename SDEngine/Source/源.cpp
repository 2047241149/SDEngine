#include "SDEngine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	
	shared_ptr<SDEngine> mSDEngine;

	mSDEngine = shared_ptr<SDEngine>(new SDEngine());
	
	if (!mSDEngine)
	{
		return 0;
	}

	//运行系统对象
	mSDEngine->Run();
	
	return 1;
}

