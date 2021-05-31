#include "Quad.h"

Quad::Quad()
{
    md3dVertexBuffer=nullptr; //���㻺��
    md3dIndexBuffer = nullptr;  //��������
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



	//��ʼ�����㻺�棬��������
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

	//���ö��㻺��
	UINT stride = sizeof(Vertex); //ÿ������Ԫ�صĿ�ȴ�С������˵ÿ������Ԫ�صĴ�С
	UINT offset = 0;
	g_pDeviceContext->IASetVertexBuffers(0, 1, &md3dVertexBuffer, &stride, &offset);

	//������������
	g_pDeviceContext->IASetIndexBuffer(md3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0); //WordΪ�����ֽ�

																			
	g_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_pDeviceContext->DrawIndexed(mIndexCount, 0, 0);

	return true;
}

bool Quad::InitializeBuffer()
{
	Vertex* vertexs = nullptr;
	WORD*indices = nullptr;  //һ����Ϊ�����ֽ� 

	mVertexCount = 4;
	mIndexCount = 6;

	//������������
	vertexs = new Vertex[mVertexCount];
	if (!vertexs)
		return false;

	//������������
	indices = new WORD[mIndexCount];
	if (!indices)
		return false;
	
	//��ʼ����������
	vertexs[0].pos = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertexs[0].texcoord = XMFLOAT2(0.0f, 0.0f);
	vertexs[1].pos = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertexs[1].texcoord = XMFLOAT2(1.0f, 0.0f);
	vertexs[2].pos = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertexs[2].texcoord = XMFLOAT2(0.0f, 1.0f);
	vertexs[3].pos = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertexs[3].texcoord = XMFLOAT2(1.0f, 1.0f);

	//����������������
	//ע�������ֶ����ж��ǲ��Ǳ���
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 1;
	indices[4] = 3;
	indices[5] = 2;


	//��һ,���(����)�������ݽṹ�������Դ���ݽṹ��,���������㻺��(�����õ��Ƕ�̬����)
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

	//�ڶ�,���(����)�������ݽṹ�������Դ���ݽṹ��,��������������
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

	//�ͷŶ����������������
	delete[]vertexs;
	vertexs = nullptr;
	delete[]indices;
	indices = nullptr;
	
	return true;
}

void Quad::ShutdownBuffer()
{
	//�ͷŶ��㻺�����������
	ReleaseCOM(md3dIndexBuffer);
	ReleaseCOM(md3dVertexBuffer);

}