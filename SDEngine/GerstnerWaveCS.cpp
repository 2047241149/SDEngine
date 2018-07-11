#include "GerstnerWaveCS.h"

const int GroundThreadSize = 16;

GerstnerWaveCS::GerstnerWaveCS(UINT iWaveWidth, UINT iWaveHeight, WCHAR* csWavePosPath, WCHAR* csWaveNormalPath, const CBGerstnerWaveNoUpdate& stWaveNoUpdate):
	m_Position(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_stWaveNoUpdate(stWaveNoUpdate)
{


	//waveWidth waveHeight 变成 2 的偶数倍
	m_nWaveWidth = (iWaveWidth % 2) ? (iWaveWidth + 1) : (iWaveWidth);
	m_nWaveHeight = (iWaveHeight % 2) ? (iWaveHeight + 1) : (iWaveHeight);

	int nWaveVertexArraySize =(m_nWaveWidth * GroundThreadSize) * (m_nWaveHeight * GroundThreadSize);

	//对应 (m_nWaveWidth * GroundThreadSize - 1) * (m_nWaveHeight * GroundThreadSize - 1)个方框
	int nWaceIndexArraySize = (m_nWaveWidth * GroundThreadSize - 1) * (m_nWaveHeight * GroundThreadSize - 1);
	m_vecWaveVertexData.resize(nWaveVertexArraySize);
	m_vecWaveIndexData.resize(nWaceIndexArraySize * 6);
	Initlize(csWavePosPath, csWaveNormalPath);
}


GerstnerWaveCS::GerstnerWaveCS(const GerstnerWaveCS& other)
{

}


GerstnerWaveCS::~GerstnerWaveCS()
{

}


void GerstnerWaveCS::UpdateWaveCB(float fCurrentTime)
{
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();

	//更新变换矩阵常量缓存的值
	//将矩阵转置,在传入常量缓存前进行转置,因为GPU对矩阵数据会自动进行一次转置
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	d3dDeviceContext->Map(m_pCBWaveUpdate, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	auto pCBCommon = (CBGerstnerWaveUpdate*)(mappedSubresource.pData);
	pCBCommon->fCurrentTime = fCurrentTime;
	pCBCommon->pad = XMFLOAT3(0.0f, 0.0f,0.0f);
	d3dDeviceContext->Unmap(m_pCBWaveUpdate, 0);
}

void GerstnerWaveCS::Render()
{
	CalculateWaveData();

	CreateAndCopyToDebugBuf();
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();

	//三角形片元
	d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//设定VertexBuffetr
	UINT stride = sizeof(VertexPCNTT); //每个顶点元素的跨度大小，或者说每个顶点元素的大小
	UINT offset = 0;
	d3dDeviceContext->IASetVertexBuffers(0, 1, &m_pWaveVertexBuffer, &stride, &offset);

	//设定IndexBuffer
	d3dDeviceContext->IASetIndexBuffer(m_pWaveIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	d3dDeviceContext->DrawIndexed(m_vecWaveIndexData.size(), 0, 0);
}

void GerstnerWaveCS::CreateBufferUAV()
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	m_pWaveDataBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;

	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;

	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();
	d3dDevice->CreateUnorderedAccessView(m_pWaveDataBuffer, &uavDesc, &m_pWaveDataUAV);
}


void GerstnerWaveCS::Initlize(WCHAR* csWavePosPath, WCHAR* csWaveNormalPath)
{
	InitializeIndexBuffer();
	CreateComputerShader(csWavePosPath, csWaveNormalPath);
	CreateConstantBuffer();
	UpdateConstanBuffer();
	CreateStructBuffer();
	CreateBufferUAV();
}

void GerstnerWaveCS::CreateComputerShader(WCHAR* csWavePosPath, WCHAR* csWaveNormalPath)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* ComputerShaderBuffer = nullptr;
	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();

	DWORD flag = D3DCOMPILE_ENABLE_STRICTNESS;

#if _DEBUG
	flag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DCompileFromFile(csWavePosPath, NULL, NULL, "WaveVertexPosUVColor_CS", "cs_5_0", flag, 0, &ComputerShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//存在错误信息
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, csWavePosPath);
		}
		//不存在错误信息,也就是没有找到Shader文件
		else
		{
			MessageBox(NULL, L"can not find VS file", L"error", MB_OK);
		}
	}

	d3dDevice->CreateComputeShader(ComputerShaderBuffer->GetBufferPointer(), ComputerShaderBuffer->GetBufferSize(),
		NULL, &m_pWavePosCS);

	ReleaseCOM(ComputerShaderBuffer);
	ReleaseCOM(errorMessage);

	//------------------------------------------------
	result = D3DCompileFromFile(csWaveNormalPath, NULL, NULL, "WaveNormalTangent_CS", "cs_5_0", flag, 0, &ComputerShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//存在错误信息
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, csWavePosPath);
		}
		//不存在错误信息,也就是没有找到Shader文件
		else
		{
			MessageBox(NULL, L"can not find VS file", L"error", MB_OK);
		}
	}

	d3dDevice->CreateComputeShader(ComputerShaderBuffer->GetBufferPointer(), ComputerShaderBuffer->GetBufferSize(),
		NULL, &m_pWaveNormalCS);


	ReleaseCOM(errorMessage);
	ReleaseCOM(ComputerShaderBuffer);
}


void GerstnerWaveCS::OutputShaderErrorMessage(ID3D10Blob* errorMessage, WCHAR* shaderFilename)
{
	char* compileErrors; 
	unsigned long bufferSize, i;
	ofstream fout;

	// 获取指向错误信息文本的指针
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 获取错误信息文本的长度
	bufferSize = errorMessage->GetBufferSize();

	// 创建一个txt,用于写入错误信息
	fout.open("shader-error.txt");

	//想txt文件写入错误信息
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// 关闭文件
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	//弹出提醒的小窗口
	MessageBox(NULL, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

	
void GerstnerWaveCS::CreateConstantBuffer()
{

	ID3D11Device* pd3dDevice = D3DClass::GetInstance()->GetDevice();

	D3D11_BUFFER_DESC cbufferDescUpdate;
	ZeroMemory(&cbufferDescUpdate, sizeof(cbufferDescUpdate));
	cbufferDescUpdate.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDescUpdate.ByteWidth = sizeof(CBGerstnerWaveUpdate);   //结构体大小,必须为16字节倍数
	cbufferDescUpdate.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDescUpdate.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbufferDescUpdate.MiscFlags = 0;
	cbufferDescUpdate.StructureByteStride = 0;
	pd3dDevice->CreateBuffer(&cbufferDescUpdate, NULL, &m_pCBWaveUpdate);


	D3D11_BUFFER_DESC cbufferDescNoUpdate;
	ZeroMemory(&cbufferDescNoUpdate, sizeof(cbufferDescNoUpdate));
	cbufferDescNoUpdate.Usage = D3D11_USAGE_DEFAULT;
	cbufferDescNoUpdate.ByteWidth = sizeof(CBGerstnerWaveNoUpdate);   //结构体大小,必须为16字节倍数
	cbufferDescNoUpdate.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDescNoUpdate.CPUAccessFlags = 0;
	cbufferDescNoUpdate.MiscFlags = 0;
	cbufferDescNoUpdate.StructureByteStride = 0;
	pd3dDevice->CreateBuffer(&cbufferDescNoUpdate, NULL, &m_pCBWaveNoUpdate);

}


void GerstnerWaveCS::CreateStructBuffer()
{
	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();
	int nWaveArraySize = m_nWaveWidth * m_nWaveHeight * GroundThreadSize * GroundThreadSize;
	m_pWaveDataBuffer = nullptr;
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = sizeof(VertexPCNTT) * nWaveArraySize;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(VertexPCNTT);
	d3dDevice->CreateBuffer(&desc, nullptr, &m_pWaveDataBuffer);
	
}

void GerstnerWaveCS::UpdateConstanBuffer()
{
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();
	m_stWaveNoUpdate.fGroundCountX = m_nWaveWidth;
	m_stWaveNoUpdate.fGroundCountY = m_nWaveHeight;
	d3dDeviceContext->UpdateSubresource(m_pCBWaveNoUpdate, 0, NULL, &m_stWaveNoUpdate, 0, 0);
}

void GerstnerWaveCS::CreateAndCopyToDebugBuf()
{
	ID3D11Device* pDevice = D3DClass::GetInstance()->GetDevice();
	ID3D11DeviceContext* d3dContext = D3DClass::GetInstance()->GetDeviceContext();
	ID3D11Buffer* debugbuf = nullptr;
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	m_pWaveDataBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (FAILED(pDevice->CreateBuffer(&desc, nullptr, &debugbuf)))
	{
		return;
	}
	d3dContext->CopyResource(debugbuf, m_pWaveDataBuffer);
	D3D11_MAPPED_SUBRESOURCE MappedResourceSrc;
	VertexPCNTT* pDataSrc;
	d3dContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResourceSrc);
	pDataSrc = (VertexPCNTT*)MappedResourceSrc.pData;
	d3dContext->Unmap(debugbuf,0);

	int nWaveArraySize = m_nWaveWidth * m_nWaveHeight * GroundThreadSize * GroundThreadSize;
	VertexPCNTT* pDataDest;
	D3D11_MAPPED_SUBRESOURCE MappedResourceDest;
	d3dContext->Map(m_pWaveVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResourceDest);
	pDataDest = (VertexPCNTT*)MappedResourceDest.pData;
	memcpy(pDataDest, pDataSrc, sizeof(VertexPCNTT) * nWaveArraySize);
	d3dContext->Unmap(m_pWaveVertexBuffer, 0);
	

	ReleaseCOM(debugbuf);
	//m_vecWaveVertexData.assign(pData, pData + nWaveArraySize);
	
}

void GerstnerWaveCS::InitializeIndexBuffer()
{
	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();

	int waveIndex = 0;

	const int WaveHeight = m_nWaveHeight * GroundThreadSize - 1;
	const int WaveWidth = m_nWaveWidth * GroundThreadSize - 1;

	int nWaveArraySize = m_nWaveWidth * m_nWaveHeight * GroundThreadSize * GroundThreadSize;
	//VertexBuffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexPCNTT) * nWaveArraySize;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	d3dDevice->CreateBuffer(&vertexBufferDesc, nullptr, &m_pWaveVertexBuffer);

	//IndexData
	for (int heightIndex = 0; heightIndex < WaveHeight; ++heightIndex)
	{
		for (int widthIndex = 0; widthIndex < WaveWidth; ++widthIndex)
		{
			//第一个三角形
			//vertex1
			WORD vertexIndex = heightIndex * (WaveWidth + 1) + widthIndex;
			m_vecWaveIndexData[waveIndex] = vertexIndex;
			++waveIndex;


			//vertex3
			vertexIndex = (heightIndex + 1) * (WaveWidth + 1) + widthIndex + 1;
			m_vecWaveIndexData[waveIndex] = vertexIndex;
			++waveIndex;

			//vertex2
			vertexIndex = heightIndex * (WaveWidth + 1) + widthIndex + 1;
			m_vecWaveIndexData[waveIndex] = vertexIndex;
			++waveIndex;



			//第二个三角形
			//vertex1
			vertexIndex = heightIndex * (WaveWidth + 1) + widthIndex;
			m_vecWaveIndexData[waveIndex] = vertexIndex;
			++waveIndex;



			//vertex4
			vertexIndex = (heightIndex + 1) * (WaveWidth + 1) + widthIndex;
			m_vecWaveIndexData[waveIndex] = vertexIndex;
			++waveIndex;

			//vertex3
			vertexIndex = (heightIndex + 1) * (WaveWidth + 1) + widthIndex + 1;
			m_vecWaveIndexData[waveIndex] = vertexIndex;
			++waveIndex;

		}
	}


	//IndexBuffer
	D3D11_BUFFER_DESC  indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(WORD) * m_vecWaveIndexData.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = &m_vecWaveIndexData[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	d3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pWaveIndexBuffer);
}

void GerstnerWaveCS::CalculateWaveData()
{
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();


	//---------------1.计算WaveData的Pos数据-------------------------
	//ComputerShader
	d3dDeviceContext->CSSetShader(m_pWavePosCS, NULL, 0);

	//ConstanBuffer
	d3dDeviceContext->CSSetConstantBuffers(0, 1, &m_pCBWaveNoUpdate);
	d3dDeviceContext->CSSetConstantBuffers(1, 1, &m_pCBWaveUpdate);

	//RWStructBuffer
	d3dDeviceContext->CSSetUnorderedAccessViews(0, 1, &m_pWaveDataUAV, nullptr);

	d3dDeviceContext->Dispatch(m_nWaveWidth, m_nWaveHeight, 1);

	//---------------2.计算计算WaveData的Normal数据------------------------
	d3dDeviceContext->CSSetShader(m_pWaveNormalCS, NULL, 0);
	//RWStructBuffer
	d3dDeviceContext->CSSetUnorderedAccessViews(0, 1, &m_pWaveDataUAV, nullptr);
	d3dDeviceContext->Dispatch(m_nWaveWidth, m_nWaveHeight, 1);
}

XMMATRIX GerstnerWaveCS::GetWorldMatrix()
{
	return XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
}