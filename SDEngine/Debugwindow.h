#pragma once
#ifndef _DEBUG_WINDOW_H
#define _DEBUG_WINDOW_H

#include<d3d11_1.h>
#include<DirectXMath.h>
#include"D3DClass.h"
#include"Macro.h"  //���������ĺ�
using namespace DirectX;

class DebugWindow
{
private:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 color;
	};
private:
	ID3D11Buffer* md3dVertexBuffer; //���㻺��
	ID3D11Buffer* md3dIndexBuffer;  //��������
	int mVertexCount;
	int mIndexCount;

	//------
	int mScrrenWidth, mScrrenHeight;
	int mBitmapWidth, mBitmapHeight;

	//PostionX ��PostionYΪDX11��Ļ����
	int mPreviousPosX, mPreviousPosY;

private:
	bool InitializeBuffer();
	void ShutdownBuffer();
	void RenderBuffers();
	bool UpdateBuffers(int positionX, int positionY);



public:
	DebugWindow(int ScrrenWidth, int ScrrenHeight, int BitmapWidth, int BitmapHeight);
	DebugWindow(const DebugWindow&);
	~DebugWindow();

public:
	//Initialize�Ǵ���Ԫ��,Render������Ԫ��,Shutdown��Release
	bool Initialize(int ScrrenWidth,int ScrrenHeight,int BitmapWidth,int BitmapHeight);
	void Shutdown();
	bool Render(
		int positionX, int positionY);

	int GetIndexCount() { return mIndexCount; } //��������ֵ DrawIndexed();
	
};
#endif 
