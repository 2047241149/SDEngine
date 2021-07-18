#include "GrussianBlurCS.h"
#include "../Texture/RWRenderTexture.h"

GrussianBlurCS::GrussianBlurCS(WCHAR* csFilenPath)
{
	Init(csFilenPath);
}

GrussianBlurCS::~GrussianBlurCS()
{
	ShutDown();
}

bool GrussianBlurCS::Init(WCHAR* csFilenPath)
{
	bool result;
	result = InitShader(csFilenPath);
	if (!result)
		return false;

	return true;
}


bool GrussianBlurCS::InitShader(WCHAR* csFilePath)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* CSBlobHorizontal;
	ID3D10Blob* CSBlobVertical;
	errorMessage = NULL;
	CSBlobHorizontal = NULL;
	CSBlobVertical = NULL;
	DWORD flag = D3DCOMPILE_ENABLE_STRICTNESS;

	#if _DEBUG
	flag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif

	//����ComputeShaderHorizontal
	result = D3DCompileFromFile(csFilePath, NULL, NULL, "CS_HorizontalBlur", "cs_5_0", flag, 0, &CSBlobHorizontal, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			//Log::LogShaderCompileInfo(errorMessage, csFilePath);
		}
		else
		{
			MessageBox(nullptr, L"can not find CS file", L"error", MB_OK);
		}
	}

	HR(g_pDevice->CreateComputeShader(CSBlobHorizontal->GetBufferPointer(), 
		CSBlobHorizontal->GetBufferSize(), nullptr, &m_pCSGrussianBlurHorizontal));


	//ComputeShaderVertical
	result = D3DCompileFromFile(csFilePath, nullptr, nullptr, "CS_VerticalBlur", "cs_5_0", flag, 0, &CSBlobVertical, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			//Log::LogShaderCompileInfo(errorMessage, csFilePath);
		}
		else
		{
			MessageBox(NULL, L"can not find CS file", L"error", MB_OK);
		}
	}

	HR(g_pDevice->CreateComputeShader(CSBlobVertical->GetBufferPointer(),
		CSBlobVertical->GetBufferSize(), NULL, &m_pCSGrussianBlurVertical));

	return true;
}

void GrussianBlurCS::ShutDown()
{
	ReleaseCOM(m_pCSGrussianBlurHorizontal);
	ReleaseCOM(m_pCSGrussianBlurVertical);
}

void GrussianBlurCS::Run(int nScreenWidth, int nScreenHeight, RWRenderTexture* pRWRenderTextureH,
	RWRenderTexture* pRWRenderTextureV,
	ID3D11ShaderResourceView* pSceneRT)
{
	const float groupSharedMemorySize = 256;

	//HorizontalBlur
	int nDispatchX = (int)ceil((float)nScreenWidth / groupSharedMemorySize);
	g_pDeviceContext->CSSetShader(m_pCSGrussianBlurHorizontal, nullptr, 0);
	g_pDeviceContext->CSSetShaderResources(0, 1, &pSceneRT);
	ID3D11UnorderedAccessView* pUAVH = pRWRenderTextureH->GetUAV();
	g_pDeviceContext->CSSetUnorderedAccessViews(0, 1, &pUAVH, nullptr);
	g_pDeviceContext->Dispatch(nDispatchX, nScreenHeight, 1);


	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	g_pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	g_pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

	//VerticalBlur
	int nDispatchY = (int)ceil((float)nScreenHeight / groupSharedMemorySize);
	g_pDeviceContext->CSSetShader(m_pCSGrussianBlurVertical, nullptr, 0);
	ID3D11ShaderResourceView* pSRV = pRWRenderTextureH->GetSRV();
	g_pDeviceContext->CSSetShaderResources(0, 1, &pSRV);
	ID3D11UnorderedAccessView* pUAVV = pRWRenderTextureV->GetUAV();
	g_pDeviceContext->CSSetUnorderedAccessViews(0, 1, &pUAVV, nullptr);
	g_pDeviceContext->Dispatch(nScreenWidth, nDispatchY, 1);


	//��ֹSRV��UAVͬʱ���У������ǲ�ͬ��Shader�׶���Դ��������
	g_pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	g_pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

}