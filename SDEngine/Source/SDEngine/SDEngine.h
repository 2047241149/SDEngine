#pragma once

#ifndef _SYSTEM_CLASS_H
#define _SYSTEM_CLASS_H
#define WIN32_LEAN_AND_MEAN  //�������Լ���Win32ͷ�ļ��Ĵ�С

#include<Windows.h>  
#include<memory>
#include "GraphicsSystem.h"
#include"FPS.h"


class SDEngine
{

private:
	LPCWSTR mApplicationName;  //Ӧ������
	HINSTANCE mHinstance; //Ӧ��ʵ�����
	HWND mHwnd; //Ӧ�ô��ھ��

private:
	//InputClass* m_Input; //������	
	shared_ptr<GraphicsSystem> mGraphicsSystem;

private:
	//֡����
	bool Tick();

	//��ʼ�����ں���
	void InitWindow(int&, int&);

	//�رմ��ں���
	void ShutdownWindow();

	/*�ر�Ӧ�ú���*/
	void ShutDown();

public:

	/*���캯��*/
	SDEngine();

	/*��������*/
	~SDEngine();

	/*��ʼ������*/
	bool Init();

	/*���к���*/
	void Run();

	/*��Ϣ��������,��������������ܵ���Ϣ�ܶഫ�ݸ�ȫ�־�̬����WndProc����,����ص�������Щ����,��ע��*/
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif 