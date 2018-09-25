
#include "ShaderManager.h"
#include "CascadeShadowMapManager.h"

ShaderManager::ShaderManager()
{
	Init();
}

ShaderManager::~ShaderManager()
{

}

ShaderManager::ShaderManager(const ShaderManager& other)
{

}

bool ShaderManager::Init()
{

 	mDiffuseNormalShader = shared_ptr<DiffuseNormalShader>(new
		DiffuseNormalShader(L"Resource/Shader/DiffuseNormalShader.fx", L"Resource/Shader/DiffuseNormalShader.fx"));

	mDiffuseNormalSpecShader = shared_ptr<DiffuseNormalSpecShader>(new
		DiffuseNormalSpecShader(L"Resource/Shader/DiffuseNormalSpecularShader.fx", L"Resource/Shader/DiffuseNormalSpecularShader.fx"));

	//�����Լ���ʼ��mPhongShader
	mDiffuseShader = shared_ptr<DiffuseShader>(new
		DiffuseShader(L"Resource/Shader/DiffuseShader.fx", L"Resource/Shader/DiffuseShader.fx"));

	//�����Լ���ʼ��mPhongShader
	mDefferDirLightShader = shared_ptr<DefferedDirLightShader>(new
		DefferedDirLightShader(L"Resource/Shader/DefferedDirLightShader.fx", L"Resource/Shader/DefferedDirLightShader.fx"));

	mDefferPointLightShader = shared_ptr<DefferedPointLightShader>(new
		DefferedPointLightShader(L"Resource/Shader/DefferedPointLightShader.fx", L"Resource/Shader/DefferedPointLightShader.fx"));

	mDefferedFinalShader = shared_ptr<DefferedFinalShader>(new
		DefferedFinalShader(L"Resource/Shader/DefferedFinalShader.fx", L"Resource/Shader/DefferedFinalShader.fx"));

	mDepthShader = shared_ptr<DepthShader>(new
		DepthShader(L"Resource/Shader/DepthShader.fx", L"Resource/Shader/DepthShader.fx"));

	mPureColorShader = shared_ptr<PureColorShader>(new
		PureColorShader(L"Resource/Shader/PureColorShader.fx", L"Resource/Shader/PureColorShader.fx"));

	mUIShader = shared_ptr<UIShader>(new
		UIShader(L"Resource/Shader/UIShader.fx", L"Resource/Shader/UIShader.fx"));

	mGraphcisBlitShader = shared_ptr<GraphcisBlitShader>(new
		GraphcisBlitShader(L"Resource/Shader/GraphicsBlitShader.fx", L"Resource/Shader/GraphicsBlitShader.fx"));
	
	mDOFShader = shared_ptr<DOFShader>(new
		DOFShader(L"Resource/Shader/DOFShader.fx", L"Resource/Shader/DOFShader.fx"));

	mBlurShader = shared_ptr<BlurShader>(new
		BlurShader(L"Resource/Shader/BlurShader.fx", L"Resource/Shader/BlurShader.fx"));

	mSSRShader = shared_ptr<SSRShader>(new
		SSRShader(L"Resource/Shader/ScreenSpaceReflectShader.fx", L"Resource/Shader/ScreenSpaceReflectShader.fx"));

	mForwardPureColorShader = shared_ptr<PureColorShader>(new
		PureColorShader(L"Resource/Shader/ForwardPureColorShader.fx", 
			L"Resource/Shader/ForwardPureColorShader.fx"));

	mDepthGetShader = shared_ptr<Shader_3D>(new Shader_3D(L"Resource/Shader/DepthGetShader.fx",
		L"Resource/Shader/DepthGetShader.fx"));

	mSSRGBufferShader = shared_ptr<SSRGBufferShader>(new SSRGBufferShader(L"Resource/Shader/SSRGBuffer.fx",
		L"Resource/Shader/SSRGBuffer.fx"));

	mWaveShader = shared_ptr<WaveShader>(new
		WaveShader(L"Resource/Shader/WaveShader.fx", L"Resource/Shader/WaveShader.fx"));

	mFxaaShader = shared_ptr<FxaaShader>(new
		FxaaShader(L"Resource/Shader/FxaaShader.fx", L"Resource/Shader/FxaaShader.fx"));

	mLightDepthShader = shared_ptr<LightDepthShader>(new
		LightDepthShader(L"Resource/Shader/LightDepthMap.fx", L"Resource/Shader/LightDepthMap.fx"));

	
	mShadowMapShader = shared_ptr<ShadowMapShader>(new
		ShadowMapShader(L"Resource/Shader/DirShadowMapShader.fx", L"Resource/Shader/DirShadowMapShader.fx"));
	return true;
}


bool ShaderManager::SetDiffuseShader(CXMMATRIX worldMatrix,ID3D11ShaderResourceView* diffsuseTexture)
{
	bool result;

	result = mDiffuseShader->SetShaderParamsExtern(worldMatrix, diffsuseTexture);
	if (!result)
	{
		MessageBox(NULL, L"DiffuseShader render ʧ��", NULL, MB_OK);
		return false;
	}

	return true;
}

bool ShaderManager::SetDiffuseNormalShader(CXMMATRIX worldMatrix,
	ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV)
{
	bool result;

	result = mDiffuseNormalShader->SetShaderParamsExtern(worldMatrix,diffuseSRV,normalSRV);
	if (!result)
	{
		MessageBox(NULL, L"DiffuseNormalShader render ʧ��", NULL, MB_OK);
		return false;
	}

	return true;
}

bool ShaderManager::SetDiffuseNormalSpecShader(CXMMATRIX worldMatrix,
	ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV,
	ID3D11ShaderResourceView* specSRV)
{
	bool result;

	result = mDiffuseNormalSpecShader->SetShaderParamsExtern(worldMatrix, diffuseSRV,
		normalSRV,specSRV);
	if (!result)
	{
		MessageBox(NULL, L"DiffuseNormalSpecShader render ʧ��", NULL, MB_OK);
		return false;
	}

	return true;
}

bool ShaderManager::SetUIShader(ID3D11ShaderResourceView* diffuseTexture)

{
	bool result;
	result = mUIShader->SetShaderParams(diffuseTexture);

	if (!result)
	{
		MessageBox(NULL, L"mUIShader render ʧ��", NULL, MB_OK);
		return false;
	}

	return true;
}



bool ShaderManager::SetDepthShader(ID3D11ShaderResourceView* depthTexture)
{
	bool result;
	result = mDepthShader->SetShaderParams(depthTexture);
	if (!result)
	{
		MessageBox(NULL, L" depth shader render ʧ��", NULL, MB_OK);
		return false;
	}

	return true;
}

shared_ptr<ShaderManager> ShaderManager::Get()
{
	if (nullptr == m_spShaderManager)
	{
		m_spShaderManager = shared_ptr<ShaderManager>(new ShaderManager());
	}

	return m_spShaderManager;
}


bool ShaderManager::SetPureColorShader(CXMMATRIX worldMatrix,  FXMVECTOR surfaceColor)
{

	bool result;
	result = mPureColorShader->SetShaderParamsExtern(worldMatrix, surfaceColor);
	if (!result)
	{
		MessageBox(NULL, L" mPureColorShader render ʧ��", NULL, MB_OK);
		return false;
	}
	return true;
}

bool ShaderManager::SetForwardPureColorShader(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor)
{
	bool result;
	result = mForwardPureColorShader->SetShaderParamsExtern(worldMatrix, surfaceColor);
	if (!result)
	{
		MessageBox(NULL, L" mForwardPureColorShader render ʧ��", NULL, MB_OK);
		return false;
	}
	return true;
}

bool ShaderManager::SetDiffuseSpecShader(CXMMATRIX worldMatrix,
	ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* specSRV)
{
	bool result;

	result = mDiffuseSpecShader->SetShaderParamsExtern(worldMatrix,
		diffuseSRV, specSRV);
	if (!result)
	{
		MessageBox(NULL, L"DiffuseNormalSpecShader render ʧ��", NULL, MB_OK);
		return false;
	}

	return true;
}


bool ShaderManager::SetGraphcisBlitShader(ID3D11ShaderResourceView* screenRT)
{
	bool result;

	result = mGraphcisBlitShader->SetShaderParams(screenRT);
	if (!result)
	{
		MessageBox(NULL, L"mGraphcisBlitShader render ʧ��", NULL, MB_OK);
		return false;
	}

	return true;
}

bool ShaderManager::SetDOFShader(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT,
	ID3D11ShaderResourceView* depthRT,
	float dofStart, float dofRange, float farPlane, float nearPlane)
{
	bool result;

	result = mDOFShader->SetShaderParams(screenRT, screenBlurRT, depthRT, 
		dofStart, dofRange, farPlane,nearPlane);
	if (!result)
	{
		MessageBox(NULL, L"DOFShader render ʧ��", NULL, MB_OK);
		return false;
	}

	return true;
}


bool ShaderManager::SetBlurShader(ID3D11ShaderResourceView* screenRT)
{
	bool result;

	result = mBlurShader->SetShaderParams(screenRT);
	if (!result)
	{
		MessageBox(NULL, L"mBlurShader render ʧ��", NULL, MB_OK);
		return false;
	}

	return true;
}

bool ShaderManager::SetSSRShader(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* arraySRV[5],
	XMFLOAT2 perspectiveValue)
{

	bool result;

	result = mSSRShader->SetShaderParams(worldMatrix, arraySRV, perspectiveValue);
	if (!result)
	{
		MessageBox(NULL, L"mSSRShader render ʧ��", NULL, MB_OK);
		return false;
	}

	return true;
}

bool ShaderManager::SetDepthGetShader(CXMMATRIX worldMatrix)
{
	bool result;
	result = mDepthGetShader->SetShaderParams(worldMatrix);
	if (!result)
	{
		MessageBox(NULL, L"DepthGetShader render ʧ��", NULL, MB_OK);
		return false;
	}

	return true;
}

bool ShaderManager::SetSSRGBufferShader(ID3D11ShaderResourceView* gBuffer[2])
{
	bool result;
	result = mSSRGBufferShader->SetShaderParams(gBuffer);
	if (!result)
	{
		MessageBox(NULL, L"SSRGBufferShader render ʧ��", NULL, MB_OK);
		return false;
	}

	return true;
}

bool ShaderManager::SetWaveShader(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor,
	ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal)
{
	bool result;
	result = mWaveShader->SetShaderParamsExtern(worldMatrix, surfaceColor, diffuse, normal);
	if (!result)
	{
		MessageBox(NULL, L"mWaveShader render ʧ��", NULL, MB_OK);
		return false;
	}
	return true;
}

bool ShaderManager::SetDefferedDirLightShader(ID3D11ShaderResourceView* gBuffer[4], int nDirLightIndex)
{
	bool result;
	result = mDefferDirLightShader->SetShaderParams(gBuffer, nDirLightIndex);
	if (!result)
	{
		MessageBox(NULL, L"mDefferDirLightShader render ʧ��", NULL, MB_OK);
		return false;
	}
	return true;
}

bool ShaderManager::SetDefferedPointLightShader(ID3D11ShaderResourceView* gBuffer[3], int nPointLightIndex)
{

	bool result;
	result = mDefferPointLightShader->SetShaderParams(gBuffer, nPointLightIndex);
	if (!result)
	{
		MessageBox(NULL, L"mDefferPointLightShader render ʧ��", NULL, MB_OK);
		return false;
	}
	return true;
}

bool ShaderManager::SetDefferedFinalShader(ID3D11ShaderResourceView* gBuffer[2])
{
	bool result;
	result = mDefferedFinalShader->SetShaderParams(gBuffer);
	if (!result)
	{
		MessageBox(NULL, L"mDefferedFinalShader render ʧ��", NULL, MB_OK);
		return false;
	}
	return true;
}


bool ShaderManager::SetFXAAShader(ID3D11ShaderResourceView* screenRT,float fScreenWidth, float fScreenHeight)
{

	bool result;
	result = mFxaaShader->SetShaderParams(screenRT, fScreenWidth, fScreenHeight);
	if (!result)
	{
		MessageBox(NULL, L"SetFXAAShader render ʧ��", NULL, MB_OK);
		return false;
	}
	return true;
}

bool ShaderManager::SetLightDepthShader(CXMMATRIX worldMatrix, CXMMATRIX lightOrthoProjMatrix)
{
	bool result;
	result = mLightDepthShader->SetShaderParamsExtern(worldMatrix, lightOrthoProjMatrix);
	if (!result)
	{
		MessageBox(NULL, L"SetLightDepthShader render ʧ��", NULL, MB_OK);
		return false;
	}
	return true;
}

bool ShaderManager::SetShadowMapShader(ID3D11ShaderResourceView* worldPosTex, CascadedShadowsManager* cascadeShadowManager)
{
	bool result;
	result = mShadowMapShader->SetShaderParams(worldPosTex, cascadeShadowManager);
	if (!result)
	{
		MessageBox(NULL, L"mShadowMapShader render ʧ��", NULL, MB_OK);
		return false;
	}
	return true;

}

shared_ptr<ShaderManager> ShaderManager::m_spShaderManager = nullptr;

