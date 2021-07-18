#pragma once

#ifndef _SYSTEM_CLASS_H
#define _SYSTEM_CLASS_H
#define WIN32_LEAN_AND_MEAN  //这个宏可以减少Win32头文件的大小

#include<Windows.h>  
#include<memory>
#include "GraphicsSystem.h"
#include"FPS.h"


class SDEngine
{

private:
	LPCWSTR mApplicationName;  //应用名字
	HINSTANCE mHinstance; //应用实例句柄
	HWND mHwnd; //应用窗口句柄

private:
	//InputClass* m_Input; //输入类	
	shared_ptr<GraphicsSystem> mGraphicsSystem;

private:
	//帧函数
	bool Tick();

	//初始化窗口函数
	void InitWindow(int&, int&);

	//关闭窗口函数
	void ShutdownWindow();

	/*关闭应用函数*/
	void ShutDown();

public:

	/*构造函数*/
	SDEngine();

	/*析构函数*/
	~SDEngine();

	/*初始化函数*/
	bool Init();

	/*运行函数*/
	void Run();

	/*消息操作函数,这里这个函数接受的消息很多传递给全局静态函数WndProc处理,这个回调函数有些特殊,请注意*/
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif 