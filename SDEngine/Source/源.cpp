#include"SystemClass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	
	shared_ptr<SystemClass> mSystemClass;

	mSystemClass = shared_ptr<SystemClass>(new SystemClass());
	
	if (!mSystemClass)
	{
		return 0;
	}

	//����ϵͳ����
	mSystemClass->Run();
	
	return 1;
}

