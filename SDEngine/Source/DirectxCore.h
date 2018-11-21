#pragma once
#ifndef DIRECTX_CORE_H
#define DIRECTX_CORE_H

#include "CoreMini.h"
#include "ImportDll.h"
#include "DirectxFrame/DirectxFrame.h"
#pragma comment(lib,"DXGI.lib")

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

	//����ģʽ
	static shared_ptr<DirectxCore> Get();

	ID3DUserDefinedAnnotation* d3dUserDefinedAnnot;

	//DirectxCore��ʼ������
	bool Init(int ScreenWidth, int ScreenHeight, bool vsync, HWND hwnd, bool fullscreen, float ScreenDepth, float ScreenNear);

public:
	//���Ƴ�������
	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	//Get����
	ID3D11Device* GetDevice() { return md3dDevice; }
	ID3D11DeviceContext* GetDeviceContext(){ return md3dImmediateContext; }
	IDXGIOutput* GetDXGIOutput() { return mAdapterOutput; }
	ID3D11RenderTargetView* GetRTV() { return md3dRenderTargetView; }
	ID3D11DepthStencilView* GetDSV() { return md3dDepthStencilView; }
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
	void RecoverDefualtRS();
	void TurnOnWireFrameRender();

	//�򿪱�Ƿ������DSS
	void TurnOnMaskReflectDSS();

	//�򿪽��з����DSS
	void TurnOnEnableReflectDSS();

	void TurnOnRenderSkyBoxDSS();

	//�޳�ǰ��
	void TurnOnCullFront();

	//�ָ�Ĭ�ϵ�
	void RecoverDefaultDSS();

	//�ص�ZBufferд
	void TurnOnDisbleZWriteDSS();

	void TurnOnLightBlend();

	void TurnOffFaceCull();

	void TurnOnMaskLightVolumeDSS();

	void TurnOnRenderLightVolumeDSS();

private:
	bool mVsyncEnable;  //�Ƿ���֡��Ⱦ
	int mVideoCardMemory; //�Կ��ڴ�
	char mVideoCardDescription[128]; //�Կ�����

private:
	static shared_ptr<DirectxCore> m_pDirectxCore;

private:
	IDXGIOutput* mAdapterOutput;
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
	ID3D11DepthStencilState* m_pDSSAddLightVolumeStencil; //��������ڹ�Դ����ڵ�DSS
	ID3D11DepthStencilState* m_pDSSRenderLightVolume; //��Ⱦ��Դ�����DSS
	ID3D11DepthStencilState* renderSkyBoxDSS;
	ID3D11RasterizerState* md3dRasterizerState; //D3D�Ĺ�դ��״̬
	ID3D11RasterizerState* md3dWireFrameRS; //D3D�Ĺ�դ��״̬
	ID3D11RasterizerState* md3dCullFrontRS; //�޳�ǰ��
	ID3D11RasterizerState* m_pTurnOffCullBackRS; //�޳�ǰ��
	ID3D11BlendState* md3dEnableBlendState; //����alpha�Ļ��״̬
	ID3D11BlendState* md3dDisableBlendState; //�ر�alpha�Ļ��״̬
	ID3D11BlendState* m_pLightBlendState;
	D3D11_VIEWPORT mViewport;

};



#define GDirectxCore (DirectxCore::Get())
#define g_pDeviceContext DirectxCore::Get()->GetDeviceContext()
#define g_pDevice  DirectxCore::Get()->GetDevice()
#define g_DXGIOutput DirectxCore::Get()->GetDXGIOutput()
#define	g_RenderMask  DirectxCore::Get()->d3dUserDefinedAnnot



#endif 
