#include "Shader.h"


const int CONSTANT_BUFFER_SIZE = 16;
const int SHADER_FLOAT_SIZE = 4;
const int SHADER_TWO_FLOAT_SIZE = SHADER_FLOAT_SIZE * 2;
const int SHADER_THREE_FLOAT_SIZE = SHADER_FLOAT_SIZE * 3;
const int SHADER_FOUR_FLOAT_SIZE = SHADER_FLOAT_SIZE * 4;
const int SHADER_MATRIX_SIZE = SHADER_FLOAT_SIZE * 16;

Shader::Shader(WCHAR* vsFilenPath, WCHAR* psFilenPath) :
	vertexShader(nullptr),
	pixelShader(nullptr),
	inputLayout(nullptr),
	m_pWrapLinearSampler(nullptr),
	m_pClampPointSampler(nullptr)
{
	Init(vsFilenPath, psFilenPath);
}


Shader::Shader(const Shader& other)
{
}


Shader::~Shader()
{
	ShutDown();
}


bool Shader::Init(WCHAR* vsFilenPath, WCHAR* psFilenPath)
{

	bool result;
	result = InitShader(vsFilenPath, psFilenPath);
	if (!result)
		return false;

	return true;
}



bool Shader::InitShader(WCHAR* VSFileName, WCHAR* PSFileName)
{
	HRESULT result;
	ID3D10Blob* pErrorMessage;
	ID3D10Blob* pVertexShaderBlob;
	ID3D10Blob* pPixelShaderBlob;

	//初始化参数
	pErrorMessage = nullptr;
	pVertexShaderBlob = nullptr;
	pPixelShaderBlob = nullptr;
	DWORD flag = D3DCOMPILE_ENABLE_STRICTNESS;

#if _DEBUG
	flag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	//编译VertexShader代码,并创建VertexShader
	result = D3DCompileFromFile(VSFileName, nullptr, nullptr, "VS", "vs_5_0", flag, 0, &pVertexShaderBlob, &pErrorMessage);
	if (FAILED(result))
	{
		//存在错误信息
		if (pErrorMessage)
		{
			Log::LogShaderCompileInfo(pErrorMessage, VSFileName);
		}
		//不存在错误信息,也就是没有找到Shader文件
		else
		{
			MessageBox(nullptr, L"can not find VS file", L"error", MB_OK);
		}
	}

	HR(g_pDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(),
		pVertexShaderBlob->GetBufferSize(), nullptr, &vertexShader));


	//编译PixelShader,并创建PixelShader
	result = D3DCompileFromFile(PSFileName, nullptr, nullptr, "PS", "ps_5_0", flag, 0,
		&pPixelShaderBlob, &pErrorMessage);

	if (FAILED(result))
	{
		//存在错误信息
		if (pErrorMessage)
		{
			Log::LogShaderCompileInfo(pErrorMessage, PSFileName);
		}
		//不存在错误信息,也就是没有找到Shader文件
		else
		{
			MessageBox(NULL, L"can not find PS file", L"error", MB_OK);
		}
	}

	HR(g_pDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), nullptr, &pixelShader));

	ID3D11ShaderReflection* vsShaderReflection = nullptr;
	HR(D3DReflect(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&vsShaderReflection));
	ID3D11ShaderReflection* psShaderReflection = nullptr;
	HR(D3DReflect(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&psShaderReflection));

	if (nullptr == vsShaderReflection || nullptr == psShaderReflection)
		return false;

	ReflectInputLayout(vsShaderReflection, pVertexShaderBlob);
	ReflectShaderConstantBuffer(vsShaderReflection);
	ReflectShaderConstantBuffer(psShaderReflection);
	ReflectShaderTexture(vsShaderReflection);
	ReflectShaderTexture(psShaderReflection);
	ReflectShaderSampler(psShaderReflection);
	CreateConstantBuffer();

	//释放Blob
	ReleaseCOM(pVertexShaderBlob);
	ReleaseCOM(pPixelShaderBlob);


	//填充采样形容结构体,并且创建采样状态
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HR(g_pDevice->CreateSamplerState(&samplerDesc, &m_pWrapLinearSampler));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	HR(g_pDevice->CreateSamplerState(&samplerDesc, &m_pClampPointSampler));

	return true;
}



void Shader::ShutDown()
{
	ReleaseCOM(inputLayout);
	ReleaseCOM(pixelShader);
	ReleaseCOM(vertexShader);
	ReleaseCOM(m_pWrapLinearSampler);
	ReleaseCOM(m_pClampPointSampler);
}


void Shader::SetShaderState()
{
	//设置顶点输入布局
	g_pDeviceContext->IASetInputLayout(inputLayout);

	//设置VertexShader和PixelShader
	g_pDeviceContext->VSSetShader(vertexShader, nullptr, 0);
	g_pDeviceContext->PSSetShader(pixelShader, nullptr, 0);

	//设置采样状态
	g_pDeviceContext->PSSetSamplers(0, 1, &m_pWrapLinearSampler);
	g_pDeviceContext->PSSetSamplers(1, 1, &m_pClampPointSampler);

}

bool Shader::ReflectShaderConstantBuffer(ID3D11ShaderReflection* reflection)
{
	D3D11_SHADER_DESC shaderDesc;
	HR(reflection->GetDesc(&shaderDesc));

	for (int cbIndex = 0; cbIndex < shaderDesc.ConstantBuffers; ++cbIndex)
	{
		int registerIndex = 0;
		ID3D11ShaderReflectionConstantBuffer* buffer = nullptr;
		buffer = reflection->GetConstantBufferByIndex(cbIndex);

		D3D11_SHADER_BUFFER_DESC shaderBufferDesc;
		buffer->GetDesc(&shaderBufferDesc);

		for (int brIndex = 0; brIndex < shaderDesc.BoundResources; ++brIndex)
		{
			D3D11_SHADER_INPUT_BIND_DESC shaderIbDesc;
			reflection->GetResourceBindingDesc(brIndex, &shaderIbDesc);

			if (0 == strcmp(shaderIbDesc.Name, shaderBufferDesc.Name))
			{
				registerIndex = shaderIbDesc.BindPoint;
				break;
			}
		}

		if (mapShaderContantBuffer.end() == mapShaderContantBuffer.find(shaderBufferDesc.Name))
		{
			shared_ptr<ShaderConstantBuffer> shaderCb =
				shared_ptr<ShaderConstantBuffer>(new ShaderConstantBuffer{ (int)shaderBufferDesc.Size, registerIndex, shaderBufferDesc.Name });

			mapShaderContantBuffer[shaderBufferDesc.Name] = shaderCb;

			for (int shaderVarIndex = 0; shaderVarIndex < shaderBufferDesc.Variables; ++shaderVarIndex)
			{
				ID3D11ShaderReflectionVariable* shaderRefVarible = buffer->GetVariableByIndex(shaderVarIndex);		
				D3D11_SHADER_VARIABLE_DESC shaderVarDesc;
				shaderRefVarible->GetDesc(&shaderVarDesc);
				if (mapShaderVariable.find(shaderVarDesc.Name) == mapShaderVariable.end())
				{
					shared_ptr<ShaderVariable> shaderVarible = shared_ptr<ShaderVariable>
						(new ShaderVariable({ (int)shaderVarDesc.StartOffset,
						(int)shaderVarDesc.Size, shaderVarDesc.Name, shaderBufferDesc.Name }));
					shaderCb->vecShaderVariableName.push_back(shaderVarDesc.Name);
					shaderCb->bNeedUpdated = true;

					ShaderVariableType variableType =  ShaderVariableType::SHADER_MATRIX;
					
					switch (shaderVarDesc.Size)
					{
					case SHADER_MATRIX_SIZE:
						variableType = ShaderVariableType::SHADER_MATRIX;
						break;

					case SHADER_FLOAT_SIZE:
						variableType = ShaderVariableType::SHADER_FLOAT;
						break;

					case SHADER_TWO_FLOAT_SIZE:
						variableType = ShaderVariableType::SHADER_FLOAT2;
						break;

					case SHADER_THREE_FLOAT_SIZE:
						variableType = ShaderVariableType::SHADER_FLOAT3;
						break;

					case SHADER_FOUR_FLOAT_SIZE:
						variableType = ShaderVariableType::SHADER_FLOAT4;
						break;
					default:
						break;
					}
					
					shaderVarible->variableType = variableType;
					shaderVarible->variablePre = malloc(shaderVarible->size);
					shaderVarible->variableCurrent = malloc(shaderVarible->size);
					ZeroMemory(shaderVarible->variableCurrent, shaderVarible->size);
					ZeroMemory(shaderVarible->variablePre, shaderVarible->size);
					mapShaderVariable[shaderVarDesc.Name] = shaderVarible;


				}
			}
		}
	}

	return true;
}


bool Shader::CreateConstantBuffer()
{
	for (auto iterator = mapShaderContantBuffer.begin(); iterator != mapShaderContantBuffer.end(); ++iterator)
	{
		shared_ptr<ShaderConstantBuffer> shaderConstantBuffer = iterator->second;
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = ((shaderConstantBuffer->size / CONSTANT_BUFFER_SIZE) + 1) * CONSTANT_BUFFER_SIZE;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(g_pDevice->CreateBuffer(&bufferDesc, NULL, &shaderConstantBuffer->constantBuffer));
	}

	return true;
 }


bool Shader::SetMatrix(const string& variableName, const CXMMATRIX& matrix)
{
	if (mapShaderVariable.find(variableName) != mapShaderVariable.end())
	{
		shared_ptr<ShaderVariable> shaderVariable = mapShaderVariable[variableName];
		if (ShaderVariableType::SHADER_MATRIX == shaderVariable->variableType && sizeof(CXMMATRIX) == shaderVariable->size)
		{
			XMMATRIX memMatrix = XMMatrixTranspose(matrix);
			memcpy(shaderVariable->variablePre, shaderVariable->variableCurrent, shaderVariable->size);
			memcpy(shaderVariable->variableCurrent, (void*)&memMatrix, shaderVariable->size);
		}
	}

	return true;
}


bool Shader::SetFloat(const string& variableName, float value)
{
	if (mapShaderVariable.find(variableName) != mapShaderVariable.end())
	{
		shared_ptr<ShaderVariable> shaderVariable = mapShaderVariable[variableName];
		if (ShaderVariableType::SHADER_FLOAT == shaderVariable->variableType && sizeof(float) == shaderVariable->size)
		{
			memcpy(shaderVariable->variablePre, shaderVariable->variableCurrent, shaderVariable->size);
			memcpy(shaderVariable->variableCurrent, (void*)&value, shaderVariable->size);
		}
	}

	return true;
}

bool Shader::SetFloat2(const string& variableName, XMFLOAT2 value)
{
	if (mapShaderVariable.find(variableName) != mapShaderVariable.end())
	{
		shared_ptr<ShaderVariable> shaderVariable = mapShaderVariable[variableName];
		if (ShaderVariableType::SHADER_FLOAT2 == shaderVariable->variableType && sizeof(XMFLOAT2) == shaderVariable->size)
		{
			memcpy(shaderVariable->variablePre, shaderVariable->variableCurrent, shaderVariable->size);
			memcpy(shaderVariable->variableCurrent, (void*)&value, shaderVariable->size);
		}
	}

	return true;
}


bool Shader::SetFloat3(const string& variableName, XMFLOAT3 value)
{
	if (mapShaderVariable.find(variableName) != mapShaderVariable.end())
	{
		shared_ptr<ShaderVariable> shaderVariable = mapShaderVariable[variableName];
		if (ShaderVariableType::SHADER_FLOAT3 == shaderVariable->variableType && sizeof(XMFLOAT3) == shaderVariable->size)
		{
			memcpy(shaderVariable->variablePre, shaderVariable->variableCurrent, shaderVariable->size);
			memcpy(shaderVariable->variableCurrent, (void*)&value, shaderVariable->size);
		}
	}

	return true;
}

bool Shader::SetFloat4(const string& variableName, XMFLOAT4 value)
{
	if (mapShaderVariable.find(variableName) != mapShaderVariable.end())
	{
		shared_ptr<ShaderVariable> shaderVariable = mapShaderVariable[variableName];
		if (ShaderVariableType::SHADER_FLOAT4 == shaderVariable->variableType && sizeof(XMFLOAT4) == shaderVariable->size)
		{
			memcpy(shaderVariable->variablePre, shaderVariable->variableCurrent, shaderVariable->size);
			memcpy(shaderVariable->variableCurrent, (void*)&value, shaderVariable->size);
		}
	}

	return true;
}


void Shader::CheckUpdateConstantBuffer()
{
	/*for (auto cbIterator = mapShaderContantBuffer.begin(); cbIterator != mapShaderContantBuffer.end(); ++cbIterator)
	{
		shared_ptr<ShaderConstantBuffer> shaderConstantBuffer = cbIterator->second;
		bool bNeedUpdate = false;
		for (auto shaderVarIterator = shaderConstantBuffer->vecShaderVariableName.begin();
			shaderVarIterator != shaderConstantBuffer->vecShaderVariableName.end(); ++shaderVarIterator)
		{
			if (mapShaderVariable.end() != mapShaderVariable.find(*shaderVarIterator))
			{
				shared_ptr<ShaderVariable> shaderVariable = mapShaderVariable[*shaderVarIterator];
				switch (shaderVariable->variableType)
				{
				case SHADER_MATRIX:
					XMMATRIX* preValue = (XMMATRIX*)shaderVariable->variablePre;
					XMMATRIX* currentValue = (XMMATRIX*)shaderVariable->variableCurrent;
					bNeedUpdate = true;
					break;
				case SHADER_FLOAT:
					float* preValue = (float*)shaderVariable->variablePre;
					float* currentValue = (float*)shaderVariable->variableCurrent;	
					break;
				case SHADER_FLOAT2:

					break;
				case SHADER_FLOAT3:
					break;
				case SHADER_FLOAT4:
					break;
				default:
					break;
				}
			}
		}
	}*/
}


void Shader::Apply()
{
	UpdateConstantBuffer();
	SetShaderState();
}

bool Shader::UpdateConstantBuffer()
{
	for (auto cbIterator = mapShaderContantBuffer.begin(); cbIterator != mapShaderContantBuffer.end(); ++cbIterator)
	{
		shared_ptr<ShaderConstantBuffer> shaderConstantBuffer = cbIterator->second;
		if (nullptr == shaderConstantBuffer)
			continue;

		if (!shaderConstantBuffer->bNeedUpdated)
		{
			g_pDeviceContext->VSSetConstantBuffers(shaderConstantBuffer->registerIndex, 1, &(shaderConstantBuffer->constantBuffer));
			g_pDeviceContext->PSSetConstantBuffers(shaderConstantBuffer->registerIndex, 1, &(shaderConstantBuffer->constantBuffer));
		}

		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		HR(g_pDeviceContext->Map(shaderConstantBuffer->constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));

		for (auto shaderVarIterator = shaderConstantBuffer->vecShaderVariableName.begin();
			shaderVarIterator != shaderConstantBuffer->vecShaderVariableName.end(); ++shaderVarIterator)
		{
			if (mapShaderVariable.end() != mapShaderVariable.find(*shaderVarIterator))
			{
				shared_ptr<ShaderVariable> shaderVariable = mapShaderVariable[*shaderVarIterator];
				if (nullptr != shaderVariable)
				{
					memcpy((unsigned char*)mappedSubresource.pData + shaderVariable->startOffset,
						shaderVariable->variableCurrent, shaderVariable->size);
				}
			}
		}
		g_pDeviceContext->Unmap(shaderConstantBuffer->constantBuffer, 0);
		g_pDeviceContext->VSSetConstantBuffers(shaderConstantBuffer->registerIndex, 1, &(shaderConstantBuffer->constantBuffer));
		g_pDeviceContext->PSSetConstantBuffers(shaderConstantBuffer->registerIndex, 1, &(shaderConstantBuffer->constantBuffer));
	}

	return true;
}


bool Shader::ReflectInputLayout(ID3D11ShaderReflection* vertexShaderReflection, ID3D10Blob* vertexShaderBlob)
{
	vector<D3D11_INPUT_ELEMENT_DESC> vecInputElementDesc;
	D3D11_SHADER_DESC vertexShaderDesc;
	HR(vertexShaderReflection->GetDesc(&vertexShaderDesc));
	
	unsigned int alignedByteOffset = 0;
	
	for (int inputParamIndex = 0; inputParamIndex < vertexShaderDesc.InputParameters; ++inputParamIndex)
	{
		D3D11_SIGNATURE_PARAMETER_DESC signatureParamDesc;
		vertexShaderReflection->GetInputParameterDesc(inputParamIndex, &signatureParamDesc);

		D3D11_INPUT_ELEMENT_DESC inputElementDesc;
		inputElementDesc.SemanticName = signatureParamDesc.SemanticName;
		inputElementDesc.SemanticIndex = signatureParamDesc.SemanticIndex;
		inputElementDesc.InputSlot = 0;
		inputElementDesc.AlignedByteOffset = alignedByteOffset;
		inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElementDesc.InstanceDataStepRate = 0;

		if (1 == signatureParamDesc.Mask)
		{
			if (D3D_REGISTER_COMPONENT_UINT32 == signatureParamDesc.ComponentType)
			{
				inputElementDesc.Format = DXGI_FORMAT_R32_UINT;
			}
			else if (D3D_REGISTER_COMPONENT_SINT32 == signatureParamDesc.ComponentType)
			{
				inputElementDesc.Format = DXGI_FORMAT_R32_SINT;
			}
			else if (D3D_REGISTER_COMPONENT_FLOAT32 == signatureParamDesc.ComponentType)
			{
				inputElementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}

			alignedByteOffset += 4;
		}
		else if (signatureParamDesc.Mask <= 3)
		{
			if (D3D_REGISTER_COMPONENT_UINT32 == signatureParamDesc.ComponentType)
			{
				inputElementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			}
			else if (D3D_REGISTER_COMPONENT_SINT32 == signatureParamDesc.ComponentType)
			{
				inputElementDesc.Format = DXGI_FORMAT_R32G32_SINT;

			}
			else if (D3D_REGISTER_COMPONENT_FLOAT32 == signatureParamDesc.ComponentType)
			{
				inputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}

			alignedByteOffset += 8;
		}
		else if (signatureParamDesc.Mask <= 7)
		{
			if (D3D_REGISTER_COMPONENT_UINT32 == signatureParamDesc.ComponentType)
			{
				inputElementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			}
			else if (D3D_REGISTER_COMPONENT_SINT32 == signatureParamDesc.ComponentType)
			{
				inputElementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			}
			else if (D3D_REGISTER_COMPONENT_FLOAT32 == signatureParamDesc.ComponentType)
			{
				inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}

			alignedByteOffset += 12;
		}
		else if(signatureParamDesc.Mask <=15 )
		{
			if (D3D_REGISTER_COMPONENT_UINT32 == signatureParamDesc.ComponentType)
			{
				inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			}
			else if (D3D_REGISTER_COMPONENT_SINT32 == signatureParamDesc.ComponentType)
			{
				inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			}
			else if (D3D_REGISTER_COMPONENT_FLOAT32 == signatureParamDesc.ComponentType)
			{
				inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			alignedByteOffset += 16;
		}

		vecInputElementDesc.push_back(inputElementDesc);
	}

	if (0 == vecInputElementDesc.size())
		return false;


	HR(g_pDevice->CreateInputLayout(&vecInputElementDesc[0], vecInputElementDesc.size(), vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(), &inputLayout));

	return true;
}

bool Shader::SetTexture(const string& variableName, ID3D11ShaderResourceView* texture)
{
	if (mapShaderTexture.end() != mapShaderTexture.find(variableName) && nullptr != texture)
	{
		shared_ptr<ShaderTexture> shaderTexture = mapShaderTexture[variableName];
		g_pDeviceContext->PSSetShaderResources(shaderTexture->bindPoint, 1, &texture);
	}
	else
	{
		return false;
	}
	return true;
}

bool Shader::SetTextureSampler(const string& variableName, ID3D11SamplerState* sampler)
{

	if (mapShaderSampler.end() != mapShaderSampler.find(variableName) && nullptr != sampler)
	{
		shared_ptr<ShaderTexture> shaderTexture = mapShaderTexture[variableName];
		g_pDeviceContext->PSSetSamplers(shaderTexture->bindPoint, 1, &sampler);
	}
	else
	{
		return false;
	}
	return true;
}

bool Shader::ReflectShaderTexture(ID3D11ShaderReflection* shaderReflection)
{
	D3D11_SHADER_DESC shaderDesc;
	HR(shaderReflection->GetDesc(&shaderDesc));
	for (int brIndex = 0; brIndex < shaderDesc.BoundResources; ++brIndex)
	{
		D3D11_SHADER_INPUT_BIND_DESC shaderIbDesc;
		shaderReflection->GetResourceBindingDesc(brIndex, &shaderIbDesc);
		if (D3D_SIT_TEXTURE == shaderIbDesc.Type)
		{
			shared_ptr<ShaderTexture> shaderTexture = shared_ptr<ShaderTexture>(new ShaderTexture());
			shaderTexture->name = shaderIbDesc.Name;
			shaderTexture->bindPoint = shaderIbDesc.BindPoint;
			if (mapShaderTexture.end() == mapShaderTexture.find(shaderIbDesc.Name))
			{
				mapShaderTexture[shaderIbDesc.Name] = shaderTexture;
			}
		}
	}
	return true;
}

bool Shader::ReflectShaderSampler(ID3D11ShaderReflection* shaderReflection)
{
	D3D11_SHADER_DESC shaderDesc;
	HR(shaderReflection->GetDesc(&shaderDesc));
	for (int brIndex = 0; brIndex < shaderDesc.BoundResources; ++brIndex)
	{
		D3D11_SHADER_INPUT_BIND_DESC shaderIbDesc;
		shaderReflection->GetResourceBindingDesc(brIndex, &shaderIbDesc);
		if (D3D_SIT_SAMPLER == shaderIbDesc.Type)
		{
			shared_ptr<ShaderSampler> shaderTexture = shared_ptr<ShaderSampler>(new ShaderSampler());
			shaderTexture->name = shaderIbDesc.Name;
			shaderTexture->bindPoint = shaderIbDesc.BindPoint;
			if (mapShaderSampler.end() == mapShaderSampler.find(shaderIbDesc.Name))
			{
				mapShaderSampler[shaderIbDesc.Name] = shaderTexture;
			}
		}
	}
	return true;
}