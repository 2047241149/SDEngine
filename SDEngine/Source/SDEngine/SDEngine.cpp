#include "SDEngine.h"
#include "WindowInfo.h"
#include <stdio.h>

//ȫ�ֱ���
static SDEngine* D3DAPP = NULL;
static const int DEFAULT_WINDOW_WIDTH = 1024;
static const int DEFAULT_WINDOW_HEIGHT = 768;

//����SystemClass������ȫ�ֻص�����
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);  //����WM_QUIT��Ϣ����Ϣ���� ���յ�WM_QUIT������Ϣѭ��
		return 0;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_KEYDOWN:
	{
		//���������ESC�������˳�
		if ((unsigned int)wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		return 0;
	}

	//��������Ϣ���͵�D3DAPPҲ����SystemClass�����MessageHandler����
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

/*�������������л����ڴ��һ��ԭ���ǽ����Դ��ڴ�Ļ�������*/
SDEngine::~SDEngine()
{
	ShutDown();
}

/*ϵͳ���ʼ������*/
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

	/*ѭ��ֱ���յ����Դ��ڵĻ���ʹ���ߵ�quit��Ϣ*/
	done = false;
	while (!done)
	{
		//����������Ϣ
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				done = true;
			}	
			TranslateMessage(&msg);
			DispatchMessage(&msg);       //����Ϣ���͵�WindProc()��
		}
		else
		{
			result = Tick();  //Frame���еĺ������������Ϸ�˳�
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

	   //��������Ϣ���͵�Ĭ�ϴ�����Ϣ����
	   default:
	   {
		   return DefWindowProc(hwnd, message, wParam, lParam); //Ϊȫ�־�̬����
	   }
	}
}



void SDEngine::InitWindow(int& ScrrenWidth, int &ScrrenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScrrenSettings;
	int posX, posY;

	//��ȡһ�������ָ����������ָ��
	D3DAPP = this;   

	//��ȡӦ��ʵ�����
	mHinstance = GetModuleHandle(NULL);

	//����Ӧ��һ������
	mApplicationName = L"Engine";

	//�趨Ҫ�������������	
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

	//ע�������
	RegisterClassEx(&wc);

	//��ȡ��Ļ�ֱ��ʵĿ�Ⱥ͸߶�
	ScrrenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScrrenHeight= GetSystemMetrics(SM_CYSCREEN);

	//ȡ�����Ƿ�Ϊȫ��Ļ
	if (FULL_SCREEN)
	{
		//���Ϊȫ��Ļ,���趨��ĻΪ�û��������󻯲���Ϊ32bit
		memset(&dmScrrenSettings, 0, sizeof(dmScrrenSettings));
		dmScrrenSettings.dmSize = sizeof(dmScrrenSettings);
		dmScrrenSettings.dmPelsWidth = (unsigned long)ScrrenWidth;
		dmScrrenSettings.dmPelsHeight = (unsigned long)ScrrenHeight;
		dmScrrenSettings.dmBitsPerPel = 32;
		dmScrrenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//�ı���ʾ�趨,����Ϊȫ��Ļ
		ChangeDisplaySettings(&dmScrrenSettings, CDS_FULLSCREEN);

		//�趨�������Ͻǵ�λ��
		posX = posY = 0;
	}
	else
	{
		//������ڻ�,�趨Ϊ1024*768�ֱ���
		ScrrenWidth = DEFAULT_WINDOW_WIDTH;
		ScrrenHeight = DEFAULT_WINDOW_HEIGHT;

		//����λ����Ļ��΢�����Ϸ�
		posX = (GetSystemMetrics(SM_CXSCREEN) - ScrrenWidth) / 2-200;
		posY = (GetSystemMetrics(SM_CYSCREEN) - ScrrenHeight) / 2-100;

	}

	//��������,���һ�ȡ���ڵľ��
	mHwnd = CreateWindowEx(WS_EX_APPWINDOW, mApplicationName, mApplicationName,
		WS_OVERLAPPEDWINDOW,
		posX, posY, ScrrenWidth, ScrrenHeight, NULL, NULL, mHinstance, NULL);

	//��������ʾ����Ļ֮��,���趨�ô���Ϊ��Ҫ���е�
	ShowWindow(mHwnd, SW_SHOW);
	SetForegroundWindow(mHwnd);
	SetFocus(mHwnd);

	//���������
	ShowCursor(false);
}

void SDEngine::ShutdownWindow()
{

	//��ʾ�����
	ShowCursor(true);

	//����뿪ȫ��Ļģʽ,�ָ���ʾ�趨
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//�Ƴ�(�ƻ�)����
	DestroyWindow(mHwnd);
	mHwnd = nullptr;

	//�Ƴ�����ʵ��
	UnregisterClass(mApplicationName, mHinstance);
	mHinstance = nullptr;

	//�ÿ�Ӧ�������
	D3DAPP = nullptr;
}