#include "Debugwindow.h"


DebugWindow::DebugWindow(int ScrrenWidth, int ScrrenHeight, int BitmapWidth, int BitmapHeight)
{
    md3dVertexBuffer = NULL;
    md3dIndexBuffer = NULL;
	mVertexCount = 0;
	mIndexCount = 0;

	Initialize(ScrrenWidth, ScrrenHeight, BitmapWidth, BitmapHeight);

}


DebugWindow::DebugWindow(const DebugWindow& other)
{

}

DebugWindow::~DebugWindow()
{

}

bool DebugWindow::Initialize(int ScrrenWidth, int ScrrenHeight, int BitmapWidth, int BitmapHeight)
{
	bool result;

	mScrrenWidth = ScrrenWidth;
	mScrrenHeight = ScrrenHeight;
	mBitmapWidth = BitmapWidth;
	mBitmapHeight = BitmapHeight;
	mPreviousPosX = -1;
	mPreviousPosY = -1;

	result = InitializeBuffer();
	if (!result)
	{
		MessageBox(NULL, L"Initialize Buffer failure", L"ERROR", MB_OK);
		return false;
	}
		

		return true;
}

void DebugWindow::Shutdown()
{
	ShutdownBuffer();
}


bool DebugWindow::Render(int positionX, int positionY)
{
	bool result;

	result = UpdateBuffers(positionX, positionY);
	if (!result)
	{
		return false;
	}

	RenderBuffers();
	return true;
}

bool DebugWindow::InitializeBuffer()
{
	Vertex* vertexs=NULL;
	WORD*indices=NULL;

	mVertexCount = 6;
	mIndexCount = 6;

	vertexs = new Vertex[mVertexCount];
	if (!vertexs)
		return false;

	indices = new WORD[mIndexCount];
	if (!indices)
		return false;
	
	memset(vertexs, 0, sizeof(Vertex)*mVertexCount);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * mVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertexs;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HR(g_pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &md3dVertexBuffer));

	D3D11_BUFFER_DESC  indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(WORD) * mIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
    HR(g_pDevice->CreateBuffer(&indexBufferDesc, &indexData, &md3dIndexBuffer));

	delete[]vertexs;
	vertexs = NULL;
	delete[]indices;
	indices = NULL;
	
	return true;
}

void DebugWindow::ShutdownBuffer()
{
	ReleaseCOM(md3dIndexBuffer);
	ReleaseCOM(md3dVertexBuffer);
}

void DebugWindow::RenderBuffers()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pDeviceContext->IASetVertexBuffers(0, 1, &md3dVertexBuffer, &stride, &offset);
	g_pDeviceContext->IASetIndexBuffer(md3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0); //WordΪ�����ֽ�
	g_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_pDeviceContext->DrawIndexed(6, 0, 0);
}






bool DebugWindow::UpdateBuffers(int positionX, int positionY)
{
	if ((positionX == mPreviousPosX)&&(positionY == mPreviousPosY))
	{
		return true;
	}

	mPreviousPosX = positionX;
	mPreviousPosY = positionY;

	float left, right, top, bottom;
	left = (float)((mScrrenWidth / 2) *-1) + (float)positionX;
	right = left + (float)mBitmapWidth;
	top = (float)(mScrrenHeight / 2) - (float)positionY;
	bottom = top - (float)mBitmapHeight;

	Vertex *vertexs;
	vertexs = new Vertex[mVertexCount];
	if (!vertexs)
	{
		return false;
	}

	vertexs[0].pos = XMFLOAT3(left, top, 0.0f);
	vertexs[0].color = XMFLOAT2(0.0f, 0.0f);

	vertexs[1].pos = XMFLOAT3(right, bottom, 0.0f);
	vertexs[1].color = XMFLOAT2(1.0f, 1.0f);

	vertexs[2].pos = XMFLOAT3(left, bottom, 0.0f);
	vertexs[2].color = XMFLOAT2(0.0f, 1.0f);

	vertexs[3].pos = XMFLOAT3(left, top, 0.0f);
	vertexs[3].color = XMFLOAT2(0.0f, 0.0f);

	vertexs[4].pos = XMFLOAT3(right, top, 0.0f);
	vertexs[4].color = XMFLOAT2(1.0f, 0.0f);

	vertexs[5].pos = XMFLOAT3(right, bottom, 0.0f);
	vertexs[5].color = XMFLOAT2(1.0f, 1.0f);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HR(g_pDeviceContext->Map(md3dVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	Vertex* verticesPtr;
	verticesPtr = (Vertex*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertexs, (sizeof(Vertex) * mVertexCount));
	g_pDeviceContext->Unmap(md3dVertexBuffer, 0);

	delete vertexs;
	vertexs = NULL;
	return true;
}