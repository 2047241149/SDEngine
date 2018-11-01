
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

 	diffuseNormalShader = shared_ptr<Shader>(new
		Shader(L"Resource/Shader/DiffuseNormalShader.fx", L"Resource/Shader/DiffuseNormalShader.fx"));

	diffuseNormalSpecShader = shared_ptr<Shader>(new
		Shader(L"Resource/Shader/DiffuseNormalSpecularShader.fx", L"Resource/Shader/DiffuseNormalSpecularShader.fx"));

	diffuseSpecShader = shared_ptr<Shader>(new
		Shader(L"Resource/Shader/DiffuseSpecularShader.fx", L"Resource/Shader/DiffuseSpecularShader.fx"));

	diffuseShader = shared_ptr<Shader>(new
		Shader(L"Resource/Shader/DiffuseShader.fx", L"Resource/Shader/DiffuseShader.fx"));


	//¥¥Ω®“‘º∞≥ı ºªØmPhongShader
	defferedDirLightShader = shared_ptr<Shader>(new
		Shader(L"Resource/Shader/DefferedDirLightShader.hlsl",
			L"Resource/Shader/DefferedDirLightShader.hlsl"));

	defferedPointLightShader = shared_ptr<Shader>(new
		Shader(L"Resource/Shader/DefferedPointLightShader.fx", L"Resource/Shader/DefferedPointLightShader.fx"));

	mDefferedFinalShader = shared_ptr<DefferedFinalShader>(new
		DefferedFinalShader(L"Resource/Shader/DefferedFinalShader.fx", L"Resource/Shader/DefferedFinalShader.fx"));

	depthShader = shared_ptr<Shader>(new
		Shader(L"Resource/Shader/DepthShader.fx", L"Resource/Shader/DepthShader.fx"));

	pureColorShader = shared_ptr<Shader>(new
		Shader(L"Resource/Shader/PureColorShader.fx", L"Resource/Shader/PureColorShader.fx"));

	uiShader = shared_ptr<Shader>(new
		Shader(L"Resource/Shader/UIShader.fx", L"Resource/Shader/UIShader.fx"));

	graphicsBlitShader = shared_ptr<Shader>(new
		Shader(L"Resource/Shader/GraphicsBlitShader.fx", L"Resource/Shader/GraphicsBlitShader.fx"));
	

	mSSRShader = shared_ptr<SSRShader>(new
		SSRShader(L"Resource/Shader/ScreenSpaceReflectShader.fx", L"Resource/Shader/ScreenSpaceReflectShader.fx"));

	forwardPureColorShader = shared_ptr<Shader>(new
		Shader(L"Resource/Shader/ForwardPureColorShader.fx",
			L"Resource/Shader/ForwardPureColorShader.fx"));

	depthGetShader = shared_ptr<Shader>(new Shader(L"Resource/Shader/DepthGetShader.fx",
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

shared_ptr<ShaderManager> ShaderManager::Get()
{
	if (nullptr == m_spShaderManager)
	{
		m_spShaderManager = shared_ptr<ShaderManager>(new ShaderManager());
	}

	return m_spShaderManager;
}


bool ShaderManager::SetSSRShader(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* arraySRV[5],
	XMFLOAT2 perspectiveValue)
{

	bool result;

	result = mSSRShader->SetShaderParams(worldMatrix, arraySRV, perspectiveValue);
	if (!result)
	{
		MessageBox(NULL, L"mSSRShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L"SSRGBufferShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L"mWaveShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L"mDefferedFinalShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L"SetFXAAShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L"SetLightDepthShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L"mShadowMapShader render  ß∞‹", NULL, MB_OK);
		return false;
	}
	return true;

}

shared_ptr<ShaderManager> ShaderManager::m_spShaderManager = nullptr;

