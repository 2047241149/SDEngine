#pragma once
#ifndef _BITMAP_CLASS_H
#define _BITMAP_CLASS_H

#include "../Common/CoreMini.h"

/*Ĭ�������ͼƬ�Ĵ�С������Ĵ��ڲ�����ô�󣬲������Ͻǵ���������WIN32����ϵ(0,0)��*/

class Quad
{
private:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 texcoord;
	};
private:
	ID3D11Buffer* md3dVertexBuffer; //���㻺��
	ID3D11Buffer* md3dIndexBuffer;  //��������
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
	//Initialize�Ǵ���Ԫ��,Render������Ԫ��,Shutdown��Release
	void Shutdown();
	bool Render();

	int GetIndexCount() { return mIndexCount; } //��������ֵ DrawIndexed();
	
};
#endif 
