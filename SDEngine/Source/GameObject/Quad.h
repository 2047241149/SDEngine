#pragma once
#ifndef _BITMAP_CLASS_H
#define _BITMAP_CLASS_H

#include "../Common/CoreMini.h"

/*默认情况下图片的大小跟传入的窗口参数这么大，并且左上角的坐标是在WIN32坐标系(0,0)处*/

class Quad
{
private:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 texcoord;
	};
private:
	ID3D11Buffer* md3dVertexBuffer; //顶点缓存
	ID3D11Buffer* md3dIndexBuffer;  //索引缓存
	int mVertexCount;
	int mIndexCount;

	//------------------
	int mScrrenWidth, mScrrenHeight;


private:
	bool InitializeBuffer();
	void ShutdownBuffer();
	bool Initialize();


public:
	Quad();
	Quad(const Quad&);
	~Quad();

public:
	//Initialize是创建元素,Render是设置元素,Shutdown是Release
	void Shutdown();
	bool Render();

	int GetIndexCount() { return mIndexCount; } //返回索引值 DrawIndexed();
	
};
#endif 
