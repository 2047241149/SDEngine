#pragma once
#ifndef _SYSTEM_CLASS_H
#define _SYSTEM_CLASS_H

#define WIN32_LEAN_AND_MEAN  //�������Լ���Win32ͷ�ļ��Ĵ�С

#include<Windows.h>  
#include<memory>
#include"GraphicsClass.h"
#include"FPS.h"

class SystemClass
{

private:
	LPCWSTR mApplicationName;  //Ӧ������
	HINSTANCE mHinstance; //Ӧ��ʵ�����
	HWND mHwnd; //Ӧ�ô��ھ��

private:
	//InputClass* m_Input; //������	
	shared_ptr<GraphicsClass> mGraphicsClass;

private:
	//֡����
	bool Frame();

	//��ʼ�����ں���
	void InitializeWindow(int&, int&);

	//�رմ��ں���
	void ShutdownWindow();

	/*�ر�Ӧ�ú���*/
	void ShutDown();

public:

	/*���캯��*/
	SystemClass();

	/*�������캯��*/
	SystemClass(const SystemClass&);

	/*��������*/
	~SystemClass();

	/*��ʼ������*/
	bool Initialize();


	/*���к���*/
	void Run();

	/*��Ϣ��������,��������������ܵ���Ϣ�ܶഫ�ݸ�ȫ�־�̬����WndProc����,����ص�������Щ����,��ע��*/
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif 
