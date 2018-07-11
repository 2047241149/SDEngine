#include "ComputerShader.h"

ComputerShader::ComputerShader(WCHAR* csFilenPath)
{
	mComputerShader = nullptr;
	mInputBufferA = nullptr;
	mInputBufferB = nullptr;
	mResultBuffer = nullptr;

	mBufferASRV = nullptr;
	mBufferBSRV = nullptr;
	mBufferResultUAV = nullptr;

	Initialize(csFilenPath);
}

ComputerShader::ComputerShader(const ComputerShader& other)
{

}


ComputerShader::~ComputerShader()
{
	Shutdown();
}

bool ComputerShader::Initialize(WCHAR* csFilenPath)
{
	CreateComputerShader(csFilenPath);
	mBufferDataA.resize(DATA_ARRAY_SIZE);
	mBufferDataB.resize(DATA_ARRAY_SIZE);
	for (int index = 0; index < DATA_ARRAY_SIZE; ++index)
	{
		mBufferDataA[index].i = 0;
		mBufferDataB[index].i = 0;
	}

	//创建Buffer
	CreateStructBuffer(sizeof(DataType),
		DATA_ARRAY_SIZE, &mBufferDataA[0], &mInputBufferA);

	CreateStructBuffer(sizeof(DataType),
		DATA_ARRAY_SIZE, &mBufferDataB[0], &mInputBufferB);

	CreateStructBuffer(sizeof(DataType),
		DATA_ARRAY_SIZE, nullptr, &mResultBuffer);

	//创建SRV
	CreatBufferSRV(mInputBufferA, &mBufferASRV);
	CreatBufferSRV(mInputBufferB, &mBufferBSRV);

	//创建UAV
	CreateBufferUAV(mResultBuffer, &mBufferResultUAV);

	return true;
}

void ComputerShader::CreateComputerShader(WCHAR* csFilenPath)
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

	result = D3DCompileFromFile(csFilenPath, NULL, NULL, "CS", "cs_5_0", flag, 0, &ComputerShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//存在错误信息
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, csFilenPath);
		}
		//不存在错误信息,也就是没有找到Shader文件
		else
		{
			MessageBox(NULL, L"can not find VS file", L"error", MB_OK);
		}
	}

	d3dDevice->CreateComputeShader(ComputerShaderBuffer->GetBufferPointer(), ComputerShaderBuffer->GetBufferSize(),
		NULL, &mComputerShader);

	ReleaseCOM(errorMessage);
	ReleaseCOM(ComputerShaderBuffer);
}
	
void ComputerShader::CreateBuffer()
{
	
}
	
void ComputerShader::Shutdown()
{
	ReleaseCOM(mComputerShader);
	ReleaseCOM(mInputBufferA);
	ReleaseCOM(mInputBufferB);
	ReleaseCOM(mResultBuffer);
	ReleaseCOM(mBufferASRV);
	ReleaseCOM(mBufferBSRV);
	ReleaseCOM(mBufferResultUAV);
}


void ComputerShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, WCHAR* shaderFilename)
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

void ComputerShader::CreateStructBuffer(UINT uElementSize, UINT uCount,
	void* pInitData, ID3D11Buffer** ppBufOut)
{
	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();
	
	*ppBufOut = nullptr;
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = uElementSize * uCount;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = uElementSize;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		d3dDevice->CreateBuffer(&desc, &InitData, ppBufOut);
	}
	else
	{
		d3dDevice->CreateBuffer(&desc, nullptr, ppBufOut);
	}
}

void ComputerShader::CreatBufferSRV(ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
{
	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();

	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	//RawBuffer創建的SRV
	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
	}
	//StructBuffer創建的SRV
	else if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	}

	d3dDevice->CreateShaderResourceView(pBuffer, &desc, ppSRVOut);
}

void ComputerShader::CreateBufferUAV(ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAV)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		uavDesc.Buffer.NumElements = descBuf.ByteWidth / 4;
	}
	else if(descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	}

	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();
	d3dDevice->CreateUnorderedAccessView(pBuffer, &uavDesc, ppUAV);
}

void ComputerShader::RunComputer(ID3D11Buffer* pCBCS, void* pCBData,
	DWORD dwNumDataBytes, UINT dispatchX, UINT dispatchY, UINT dispatchZ)
{
	const UINT nNumViews = 2;
	ID3D11ShaderResourceView* arraySRV[nNumViews];
	arraySRV[0] = mBufferASRV;
	arraySRV[1] = mBufferBSRV;
	ID3D11DeviceContext* pDeviceContext = D3DClass::GetInstance()->GetDeviceContext();
	pDeviceContext->CSSetShader(mComputerShader, nullptr, 0);
	pDeviceContext->CSSetShaderResources(0, nNumViews, arraySRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, &mBufferResultUAV, nullptr);
	if (pCBCS&&pCBData)
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		pDeviceContext->Map(pCBCS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		memcpy(MappedResource.pData, pCBData, dwNumDataBytes);
		pDeviceContext->Unmap(pCBCS, 0);
		pDeviceContext->CSSetConstantBuffers(0, 1, &pCBCS);
	}

	pDeviceContext->Dispatch(dispatchX, dispatchY, dispatchZ);

	CreateAndCopyToDebugBuf();
}

void ComputerShader::CreateAndCopyToDebugBuf()
{
	ID3D11Device* pDevice = D3DClass::GetInstance()->GetDevice();
	ID3D11DeviceContext* d3dContext = D3DClass::GetInstance()->GetDeviceContext();
	ID3D11Buffer* debugbuf = nullptr;
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	
	mResultBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if(FAILED(pDevice->CreateBuffer(&desc,nullptr,&debugbuf)))
	{
		return;
	}
	d3dContext->CopyResource(debugbuf, mResultBuffer);
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	DataType* pData;
	d3dContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);

	pData = (DataType*)MappedResource.pData;

	for (int index = 0; index < DATA_ARRAY_SIZE; ++index)
	{
		std::cout << "   " << pData[index].column<<"     "<< pData[index].row << "      "<<pData[index].i << std::endl;
	}

}