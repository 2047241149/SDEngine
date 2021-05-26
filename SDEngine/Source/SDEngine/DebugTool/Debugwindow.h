#pragma once
#ifndef _DEBUG_WINDOW_H
#define _DEBUG_WINDOW_H

#include "../Common/CoreMini.h"

class DebugWindow
{
private:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 color;
	};
private:
	ID3D11Buffer* md3dVertexBuffer; //顶点缓存
	ID3D11Buffer* md3dIndexBuffer;  //索引缓存
	int mVertexCount;
	int mIndexCount;

	//------
	int mScrrenWidth, mScrrenHeight;
	int mBitmapWidth, mBitmapHeight;

	//PostionX 和PostionY为DX11屏幕坐标
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
	//Initialize是创建元素,Render是设置元素,Shutdown是Release
	bool Initialize(int ScrrenWidth,int ScrrenHeight,int BitmapWidth,int BitmapHeight);
	void Shutdown();
	bool Render(
		int positionX, int positionY);

	int GetIndexCount() { return mIndexCount; } //返回索引值 DrawIndexed();
	
};
#endif 