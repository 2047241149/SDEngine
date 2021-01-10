#include"Quad.h"

Quad::Quad()
{
    md3dVertexBuffer=nullptr; //顶点缓存
    md3dIndexBuffer = nullptr;  //索引缓存
	mVertexCount = 0;
	mIndexCount = 0;
	Initialize();
}


Quad::Quad(const Quad& other)
{

}

Quad::~Quad()
{
	Shutdown();
}

bool Quad::Initialize()
{
	bool result;



	//初始化顶点缓存，索引缓存
	result = InitializeBuffer();
	if (!result)
	{
		MessageBox(nullptr, L"Initialize Buffer failure", L"ERROR", MB_OK);
		return false;
	}
		
	return true;
}

void Quad::Shutdown()
{
	ShutdownBuffer();
}


bool Quad::Render()
{

	//设置顶点缓存
	UINT stride = sizeof(Vertex); //每个顶点元素的跨度大小，或者说每个顶点元素的大小
	UINT offset = 0;
	g_pDeviceContext->IASetVertexBuffers(0, 1, &md3dVertexBuffer, &stride, &offset);

	//设置索引缓存
	g_pDeviceContext->IASetIndexBuffer(md3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0); //Word为两个字节

																			
	g_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_pDeviceContext->DrawIndexed(mIndexCount, 0, 0);

	return true;
}

bool Quad::InitializeBuffer()
{
	Vertex* vertexs = nullptr;
	WORD*indices = nullptr;  //一个字为两个字节 

	mVertexCount = 4;
	mIndexCount = 6;

	//创建顶点数组
	vertexs = new Vertex[mVertexCount];
	if (!vertexs)
		return false;

	//创建索引数组
	indices = new WORD[mIndexCount];
	if (!indices)
		return false;
	
	//初始化顶点数组
	vertexs[0].pos = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertexs[0].texcoord = XMFLOAT2(0.0f, 0.0f);
	vertexs[1].pos = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertexs[1].texcoord = XMFLOAT2(1.0f, 0.0f);
	vertexs[2].pos = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertexs[2].texcoord = XMFLOAT2(0.0f, 1.0f);
	vertexs[3].pos = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertexs[3].texcoord = XMFLOAT2(1.0f, 1.0f);

	//赋予索引数组数据
	//注意用左手定则判定是不是背面
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 1;
	indices[4] = 3;
	indices[5] = 2;


	//第一,填充(顶点)缓存形容结构体和子资源数据结构体,并创建顶点缓存(这里用的是动态缓存)
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * mVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertexs;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HR(g_pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &md3dVertexBuffer));

	//第二,填充(索引)缓存形容结构体和子资源数据结构体,并创建索引缓存
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

	//释放顶点数组和索引数组
	delete[]vertexs;
	vertexs = nullptr;
	delete[]indices;
	indices = nullptr;
	
	return true;
}

void Quad::ShutdownBuffer()
{
	//释放顶点缓存和索引缓存
	ReleaseCOM(md3dIndexBuffer);
	ReleaseCOM(md3dVertexBuffer);

}
