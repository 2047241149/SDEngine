#include"SystemClass.h"


//ȫ�ֱ���
static SystemClass* D3DAPP = NULL;


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



SystemClass::SystemClass()
{
	Initialize();
}

SystemClass::SystemClass(const SystemClass& sys)
{

}

/*�������������л����ڴ��һ��ԭ���ǽ����Դ��ڴ�Ļ�������*/
SystemClass::~SystemClass()
{
	ShutDown();
}

/*ϵͳ���ʼ������*/
bool SystemClass::Initialize()
{
	int ScreenWidth, ScreenHeight;

	//------��һ,��Դ�����------
	/*��ʼ����Ļ��Ⱥ͸߶�*/
	/*��ʼ��ϵͳ��Ĵ���*/
	InitializeWindow(ScreenWidth, ScreenHeight);

	//------�ڶ�,���ͼ����------
	/*����ͼ�������*/
	mGraphicsClass = shared_ptr<GraphicsClass>(new GraphicsClass(ScreenWidth,ScreenHeight,mHwnd,mHinstance));
	if (!mGraphicsClass)
	{
		MessageBox(NULL, L"mGraphicsClass initialzie failure", NULL, MB_OK);
		return false;
	}


	return true;
}

void SystemClass::ShutDown()
{
	this->ShutdownWindow();
}

void SystemClass::Run()
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
			result = Frame();  //Frame���еĺ������������Ϸ�˳�
			if (!result)
			{
				done = true;
			}
		}
	}
}

bool SystemClass::Frame()
{

	mGraphicsClass->Frame();

	//����ÿ֡��ͼ����Ⱦ
	mGraphicsClass->Render();

	int fps = FPS::GetInstance()->GetFPS();
	string fpsStr;
	int2str(fps, fpsStr);
	string title = "SDENGINE     FPS = " + fpsStr;
	SetWindowText(mHwnd, Str2Wstr(title).c_str());

	return true;
}


LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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



void SystemClass::InitializeWindow(int& ScrrenWidth, int &ScrrenHeight)
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
		ScrrenWidth = 1024;
		ScrrenHeight = 768;

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

void SystemClass::ShutdownWindow()
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
	mHwnd = NULL;

	//�Ƴ�����ʵ��
	UnregisterClass(mApplicationName, mHinstance);
	mHinstance = NULL;

	//�ÿ�Ӧ�������
	D3DAPP = NULL;
}

