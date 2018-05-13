#pragma once
#ifndef D3D_CLASS_H
#define D3D_CLASS_H

#include"Macro.h"  //包含辅助的宏
#include<Windows.h>
#include<d3d11_1.h>
#include<DirectXMath.h>
#include<iostream>
#include<istream>
#include<ostream>
#include<strstream>
#include<fstream>
#include"ImportDll.h"
#pragma comment(lib,"DXGI.lib")
#include<memory>
using namespace std;
using namespace DirectX;

class D3DClass
{
private:
	bool mVsyncEnable;  //是否限帧渲染
	int mVideoCardMemory; //显卡内存
	char mVideoCardDescription[128]; //显卡名字
	
private:
	static shared_ptr<D3DClass> mD3D;
    
private:
	
	ID3D11Device* md3dDevice;//D3D11设备
	ID3D11DeviceContext* md3dImmediateContext;//D3D11设备上下文
	IDXGISwapChain* md3dSwapChain;//D3D交换链
	ID3D11RenderTargetView* md3dRenderTargetView; //D3D11渲染目标视图
	ID3D11DepthStencilView* md3dDepthStencilView; //D3D11深度(模板)视图
	ID3D11Texture2D* md3dDepthStencilBuffer; //D3D11的“DepthStencil缓存”
	ID3D11DepthStencilState* md3dDepthStencilState; //深度(模板)缓存状态
	ID3D11DepthStencilState* md3dDisableDepthStencilState; //深度测试失效的缓存状态
	ID3D11DepthStencilState* md3dDisableZWriteDSS; //深度写无效的缓存状态
	ID3D11DepthStencilState* md3dDSSMaskReflect;  //标记反射面
	ID3D11DepthStencilState* md3dDSSEnableReflect; //进行反射面
	ID3D11RasterizerState* md3dRasterizerState; //D3D的光栅化状态
	ID3D11RasterizerState* md3dWireFrameRS; //D3D的光栅化状态
	ID3D11RasterizerState* md3dCullFrontRS; //剔除前面
	ID3D11BlendState* md3dEnableBlendState; //开启alpha的混合状态
	ID3D11BlendState* md3dDisableBlendState; //关闭alpha的混合状态
	D3D11_VIEWPORT mViewport;

private:
	void ShutDown();

public:
	//构造,拷贝构造,析构函数
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	//单例模式
	static D3DClass* GetInstance();

	//D3DClass初始化函数
	bool Initialize(int ScreenWidth, int ScreenHeight, bool vsync, HWND hwnd, bool fullscreen, float ScreenDepth, float ScreenNear);

public:
	//绘制场景函数
	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	//Get函数
	ID3D11Device* GetDevice() { return md3dDevice; }
	ID3D11DeviceContext* GetDeviceContext(){ return md3dImmediateContext; }
	ID3D11RenderTargetView* GetRTV() { return md3dRenderTargetView; }
	ID3D11DepthStencilView* GetSDV() { return md3dDepthStencilView; }
	D3D11_VIEWPORT GetViewPort() { return mViewport; }

	void GetVideoCardInfo(char*, int&); //获取显卡信息

	//Set函数
	void SetBackBufferRender();  //设置背后缓存作为渲染目标
	void SetViewPort(); //重置该视口

	//打开和关闭alpha混合函数
	void TurnOnAlphaBlend();
	void TurnOffAlphaBlend();

	//打开和关闭ZTest
	void TurnOnZBuffer();
	void TurnOffZBuffer();
	
	//打开Solid渲染和线框渲染
	void TurnOnSolidRender();
	void TurnOnWireFrameRender();

	//打开标记反射面的DSS
	void TurnOnMaskReflectDSS();

	//打开进行反射的DSS
	void TurnOnEnableReflectDSS();

	//剔除前面
	void TurnOnCullFront();

	//恢复默认的
	void RecoverDefaultDSS();

	//关掉ZBuffer写
	void TurnOnDisbleZWriteDSS();
};
#endif // !D3D_CLASS_H
