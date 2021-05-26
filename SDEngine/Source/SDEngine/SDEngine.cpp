#include "SDEngine.h"
#include "WindowInfo.h"
#include <stdio.h>

//全局变量
static SDEngine* D3DAPP = NULL;
static const int DEFAULT_WINDOW_WIDTH = 1024;
static const int DEFAULT_WINDOW_HEIGHT = 768;

//接受SystemClass类对象的全局回调函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);  //发送WM_QUIT消息到消息队列 接收到WM_QUIT跳出消息循环
		return 0;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_KEYDOWN:
	{
		//如果按下是ESC键，则退出
		if ((unsigned int)wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		return 0;
	}

	//将其它消息传送到D3DAPP也就是SystemClass对象的MessageHandler函数
	default:
		return D3DAPP->MessageHandler(hwnd, message, wParam, lParam);

	}
}



SDEngine::SDEngine()
{
	Init();
}

SDEngine::SDEngine(const SDEngine& sys)
{

}

/*不在析构函数中回收内存的一点原因是谨慎对待内存的回收问题*/
SDEngine::~SDEngine()
{
	ShutDown();
}

/*系统类初始化函数*/
bool SDEngine::Init()
{
	int ScreenWidth, ScreenHeight;

	//init windows
	InitWindow(ScreenWidth, ScreenHeight);
	GWindowInfo->Init(mHwnd, mHinstance, ScreenWidth, ScreenHeight);

	mGraphicsSystem = shared_ptr<GraphicsSystem>(new GraphicsSystem());
	if (!mGraphicsSystem)
	{
		MessageBox(NULL, L"mGraphicsSystem init failure", NULL, MB_OK);
		return false;
	}


	return true;
}

void SDEngine::ShutDown()
{
	this->ShutdownWindow();
}

void SDEngine::Run()
{
	MSG msg = { 0 };
	bool done, result;

	/*循环直到收到来自窗口的或者使用者的quit消息*/
	done = false;
	while (!done)
	{
		//操作窗口消息
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				done = true;
			}	
			TranslateMessage(&msg);
			DispatchMessage(&msg);       //把消息发送到WindProc()中
		}
		else
		{
			result = Tick();  //Frame运行的函数可能造成游戏退出
			if (!result)
			{
				done = true;
			}
		}
	}
}

bool SDEngine::Tick()
{
	int fps = GFPS->GetFPS();
	float deltatTime = 1.0 / (float(fps) + 0.001f);

	//Logic
	mGraphicsSystem->Tick(deltatTime);

	//Graphics
	mGraphicsSystem->Render();

	string fpsStr;
	int2str(fps, fpsStr);
	string title = "SDENGINE     FPS = " + fpsStr;
	SetWindowText(mHwnd, Str2Wstr(title).c_str());
	return true;
}


LRESULT CALLBACK SDEngine::MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
  
	   case WM_KEYDOWN:
	  {
		  return 0;
	  }
	   case WM_KEYUP:
	   {
		   return 0;
	   }

	   //其它的消息被送到默认处理消息函数
	   default:
	   {
		   return DefWindowProc(hwnd, message, wParam, lParam); //为全局静态函数
	   }
	}
}



void SDEngine::InitWindow(int& ScrrenWidth, int &ScrrenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScrrenSettings;
	int posX, posY;

	//获取一个额外的指向这个对象的指针
	D3DAPP = this;   

	//获取应用实例句柄
	mHinstance = GetModuleHandle(NULL);

	//给予应用一个名字
	mApplicationName = L"Engine";

	//设定要创建的类的属性	
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mHinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = mApplicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//注册这个类
	RegisterClassEx(&wc);

	//获取屏幕分辨率的宽度和高度
	ScrrenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScrrenHeight= GetSystemMetrics(SM_CYSCREEN);

	//取决于是否为全屏幕
	if (FULL_SCREEN)
	{
		//如果为全屏幕,则设定屏幕为用户桌面的最大化并且为32bit
		memset(&dmScrrenSettings, 0, sizeof(dmScrrenSettings));
		dmScrrenSettings.dmSize = sizeof(dmScrrenSettings);
		dmScrrenSettings.dmPelsWidth = (unsigned long)ScrrenWidth;
		dmScrrenSettings.dmPelsHeight = (unsigned long)ScrrenHeight;
		dmScrrenSettings.dmBitsPerPel = 32;
		dmScrrenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//改变显示设定,设置为全屏幕
		ChangeDisplaySettings(&dmScrrenSettings, CDS_FULLSCREEN);

		//设定窗口左上角的位置
		posX = posY = 0;
	}
	else
	{
		//如果窗口化,设定为1024*768分辨率
		ScrrenWidth = DEFAULT_WINDOW_WIDTH;
		ScrrenHeight = DEFAULT_WINDOW_HEIGHT;

		//窗口位于屏幕稍微的左上方
		posX = (GetSystemMetrics(SM_CXSCREEN) - ScrrenWidth) / 2-200;
		posY = (GetSystemMetrics(SM_CYSCREEN) - ScrrenHeight) / 2-100;

	}

	//创建窗口,并且获取窗口的句柄
	mHwnd = CreateWindowEx(WS_EX_APPWINDOW, mApplicationName, mApplicationName,
		WS_OVERLAPPEDWINDOW,
		posX, posY, ScrrenWidth, ScrrenHeight, NULL, NULL, mHinstance, NULL);

	//将窗口显示于屏幕之上,并设定该窗口为主要集中点
	ShowWindow(mHwnd, SW_SHOW);
	SetForegroundWindow(mHwnd);
	SetFocus(mHwnd);

	//隐藏鼠标光标
	ShowCursor(false);
}

void SDEngine::ShutdownWindow()
{

	//显示鼠标光标
	ShowCursor(true);

	//如果离开全屏幕模式,恢复显示设定
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//移除(破坏)窗口
	DestroyWindow(mHwnd);
	mHwnd = nullptr;

	//移除程序实例
	UnregisterClass(mApplicationName, mHinstance);
	mHinstance = nullptr;

	//置空应用类对象
	D3DAPP = nullptr;
}