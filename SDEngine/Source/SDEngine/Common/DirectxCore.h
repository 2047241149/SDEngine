#pragma once
#ifndef DIRECTX_CORE_H
#define DIRECTX_CORE_H

#include<d3d11.h>
#include<d3d11_1.h>
#include<d3d11_2.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<memory.h>
#include<iostream>
#include "DirectxFrame.h"

using namespace std;
using namespace DirectX;

class DirectxCore
{
private:
	void ShutDown();

public:
	DirectxCore();
	DirectxCore(const DirectxCore&);
	~DirectxCore();

	static shared_ptr<DirectxCore> Get();

	ID3DUserDefinedAnnotation* d3dUserDefinedAnnot;

	bool Init(bool vsync, bool fullscreen);

public:
	//绘制场景函数
	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	//Get函数
	ID3D11Device* GetDevice() { return md3dDevice; }
	ID3D11DeviceContext* GetDeviceContext(){ return md3dImmediateContext; }
	IDXGIOutput* GetDXGIOutput() { return mAdapterOutput; }
	ID3D11RenderTargetView* GetRTV() { return md3dRenderTargetView; }
	ID3D11DepthStencilView* GetDSV() { return md3dDepthStencilView; }
	D3D11_VIEWPORT GetViewPort() { return mViewport; }

	void GetVideoCardInfo(char*, int&);

	//Set函数
	void SetBackBufferRender();  //设置背后缓存作为渲染目标
	void SetDefualtViewPort(); //重置该视口

	//打开和关闭alpha混合函数
	void TurnOnAlphaBlend();
	void TurnOffAlphaBlend();

	//打开和关闭ZTest
	void TurnOnZBuffer();
	void TurnOffZBuffer();
	
	//打开Solid渲染和线框渲染
	void RecoverDefualtRS();
	void TurnOnWireFrameRender();

	//打开标记反射面的DSS
	void TurnOnMaskReflectDSS();

	//打开进行反射的DSS
	void TurnOnEnableReflectDSS();

	void TurnOnRenderSkyBoxDSS();

	//剔除前面
	void TurnOnCullFront();

	//恢复默认的
	void RecoverDefaultDSS();

	void TurnOnPreDepthDSS();

	//关掉ZBuffer写
	void TurnOnDisbleZWriteDSS();

	void TurnOnLightBlend();

	void TurnOffFaceCull();

	void TurnOnMaskLightVolumeDSS();

	void TurnOnRenderLightVolumeDSS();

	void GenerateMips(ID3D11ShaderResourceView* srv)
	{
		md3dImmediateContext->GenerateMips(srv);
	}

private:
	bool mVsyncEnable;  //是否限帧渲染
	int mVideoCardMemory; //显卡内存
	char mVideoCardDescription[128]; //显卡名字

private:
	static shared_ptr<DirectxCore> m_pDirectxCore;

private:
	IDXGIOutput* mAdapterOutput;
	ID3D11Device* md3dDevice;//D3D11设备
	ID3D11DeviceContext* md3dImmediateContext;//D3D11设备上下文
	IDXGISwapChain* md3dSwapChain;//D3D交换链
	ID3D11RenderTargetView* md3dRenderTargetView; //D3D11渲染目标视图
	ID3D11DepthStencilView* md3dDepthStencilView; //D3D11深度(模板)视图
	ID3D11Texture2D* md3dDepthStencilBuffer; //D3D11的“DepthStencil缓存”
	ID3D11DepthStencilState* md3dDepthStencilState; //深度(模板)缓存状态
	ID3D11DepthStencilState* md3dDisableDepthStencilState; //深度测试失效的缓存状态
	ID3D11DepthStencilState* mEqualDepthStencilState; //深度(模板)缓存状态
	ID3D11DepthStencilState* md3dDisableZWriteDSS; //深度写无效的缓存状态
	ID3D11DepthStencilState* md3dDSSMaskReflect;  //标记反射面
	ID3D11DepthStencilState* md3dDSSEnableReflect; //进行反射面
	ID3D11DepthStencilState* m_pDSSAddLightVolumeStencil; //标记像素在光源体积内的DSS
	ID3D11DepthStencilState* m_pDSSRenderLightVolume; //渲染光源体积的DSS
	ID3D11DepthStencilState* renderSkyBoxDSS;
	ID3D11RasterizerState* md3dRasterizerState; //D3D的光栅化状态
	ID3D11RasterizerState* md3dWireFrameRS; //D3D的光栅化状态
	ID3D11RasterizerState* md3dCullFrontRS; //剔除前面
	ID3D11RasterizerState* m_pTurnOffCullBackRS; //剔除前面
	ID3D11BlendState* md3dEnableBlendState; //开启alpha的混合状态
	ID3D11BlendState* md3dDisableBlendState; //关闭alpha的混合状态
	ID3D11BlendState* m_pLightBlendState;
	D3D11_VIEWPORT mViewport;
};



#define GDirectxCore (DirectxCore::Get())
#define g_pDeviceContext DirectxCore::Get()->GetDeviceContext()
#define g_pDevice  DirectxCore::Get()->GetDevice()
#define g_DXGIOutput DirectxCore::Get()->GetDXGIOutput()
#define	g_RenderMask  DirectxCore::Get()->d3dUserDefinedAnnot



#endif 
