#pragma once
#ifndef D3D_CLASS_H
#define D3D_CLASS_H

#include"Macro.h"  //���������ĺ�
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
	bool mVsyncEnable;  //�Ƿ���֡��Ⱦ
	int mVideoCardMemory; //�Կ��ڴ�
	char mVideoCardDescription[128]; //�Կ�����
	
private:
	static shared_ptr<D3DClass> mD3D;
    
private:
	
	ID3D11Device* md3dDevice;//D3D11�豸
	ID3D11DeviceContext* md3dImmediateContext;//D3D11�豸������
	IDXGISwapChain* md3dSwapChain;//D3D������
	ID3D11RenderTargetView* md3dRenderTargetView; //D3D11��ȾĿ����ͼ
	ID3D11DepthStencilView* md3dDepthStencilView; //D3D11���(ģ��)��ͼ
	ID3D11Texture2D* md3dDepthStencilBuffer; //D3D11�ġ�DepthStencil���桱
	ID3D11DepthStencilState* md3dDepthStencilState; //���(ģ��)����״̬
	ID3D11DepthStencilState* md3dDisableDepthStencilState; //��Ȳ���ʧЧ�Ļ���״̬
	ID3D11DepthStencilState* md3dDisableZWriteDSS; //���д��Ч�Ļ���״̬
	ID3D11DepthStencilState* md3dDSSMaskReflect;  //��Ƿ�����
	ID3D11DepthStencilState* md3dDSSEnableReflect; //���з�����
	ID3D11RasterizerState* md3dRasterizerState; //D3D�Ĺ�դ��״̬
	ID3D11RasterizerState* md3dWireFrameRS; //D3D�Ĺ�դ��״̬
	ID3D11RasterizerState* md3dCullFrontRS; //�޳�ǰ��
	ID3D11BlendState* md3dEnableBlendState; //����alpha�Ļ��״̬
	ID3D11BlendState* md3dDisableBlendState; //�ر�alpha�Ļ��״̬
	D3D11_VIEWPORT mViewport;

private:
	void ShutDown();

public:
	//����,��������,��������
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	//����ģʽ
	static D3DClass* GetInstance();

	//D3DClass��ʼ������
	bool Initialize(int ScreenWidth, int ScreenHeight, bool vsync, HWND hwnd, bool fullscreen, float ScreenDepth, float ScreenNear);

public:
	//���Ƴ�������
	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	//Get����
	ID3D11Device* GetDevice() { return md3dDevice; }
	ID3D11DeviceContext* GetDeviceContext(){ return md3dImmediateContext; }
	ID3D11RenderTargetView* GetRTV() { return md3dRenderTargetView; }
	ID3D11DepthStencilView* GetSDV() { return md3dDepthStencilView; }
	D3D11_VIEWPORT GetViewPort() { return mViewport; }

	void GetVideoCardInfo(char*, int&); //��ȡ�Կ���Ϣ

	//Set����
	void SetBackBufferRender();  //���ñ��󻺴���Ϊ��ȾĿ��
	void SetViewPort(); //���ø��ӿ�

	//�򿪺͹ر�alpha��Ϻ���
	void TurnOnAlphaBlend();
	void TurnOffAlphaBlend();

	//�򿪺͹ر�ZTest
	void TurnOnZBuffer();
	void TurnOffZBuffer();
	
	//��Solid��Ⱦ���߿���Ⱦ
	void TurnOnSolidRender();
	void TurnOnWireFrameRender();

	//�򿪱�Ƿ������DSS
	void TurnOnMaskReflectDSS();

	//�򿪽��з����DSS
	void TurnOnEnableReflectDSS();

	//�޳�ǰ��
	void TurnOnCullFront();

	//�ָ�Ĭ�ϵ�
	void RecoverDefaultDSS();

	//�ص�ZBufferд
	void TurnOnDisbleZWriteDSS();
};
#endif // !D3D_CLASS_H
