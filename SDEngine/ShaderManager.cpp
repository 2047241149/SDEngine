
#include "ShaderManager.h"

ShaderManager::ShaderManager()
{
	Initialize();
}

ShaderManager::~ShaderManager()
{

}

ShaderManager::ShaderManager(const ShaderManager& other)
{

}

bool ShaderManager::Initialize()
{

	mDiffuseNormalShader = shared_ptr<DiffuseNormalShader>(new
		DiffuseNormalShader(L"Shader/DiffuseNormalShader.fx", L"Shader/DiffuseNormalShader.fx"));

	mDiffuseNormalSpecShader = shared_ptr<DiffuseNormalSpecShader>(new
		DiffuseNormalSpecShader(L"Shader/DiffuseNormalSpecularShader.fx", L"Shader/DiffuseNormalSpecularShader.fx"));


	//¥¥Ω®“‘º∞≥ı ºªØmPhongShader
	mDiffuseShader = shared_ptr<DiffuseShader>(new
		DiffuseShader(L"Shader/DiffuseShader.fx", L"Shader/DiffuseShader.fx"));

	//¥¥Ω®“‘º∞≥ı ºªØmPhongShader
	mDefferLightingShader = shared_ptr<DefferLighingShader>(new 
		DefferLighingShader(L"Shader/DefferLightingShader.fx", L"Shader/DefferLightingShader.fx"));

	mDepthShader = shared_ptr<DepthShader>(new
		DepthShader(L"Shader/DepthShader.fx", L"Shader/DepthShader.fx"));

	mPureColorShader = shared_ptr<PureColorShader>(new
		PureColorShader(L"Shader/PureColorShader.fx", L"Shader/PureColorShader.fx"));

	mUIShader = shared_ptr<UIShader>(new
		UIShader(L"Shader/UIShader.fx", L"Shader/UIShader.fx"));

	mGraphcisBlitShader = shared_ptr<GraphcisBlitShader>(new
		GraphcisBlitShader(L"Shader/GraphicsBlitShader.fx", L"Shader/GraphicsBlitShader.fx"));
	
	mDOFShader = shared_ptr<DOFShader>(new
		DOFShader(L"Shader/DOFShader.fx", L"Shader/DOFShader.fx"));


	mBlurShader = shared_ptr<BlurShader>(new
		BlurShader(L"Shader/BlurShader.fx", L"Shader/BlurShader.fx"));

	mSSRShader = shared_ptr<SSRShader>(new
		SSRShader(L"Shader/ScreenSpaceReflectShader.fx", L"Shader/ScreenSpaceReflectShader.fx"));

	mForwardPureColorShader = shared_ptr<PureColorShader>(new
		PureColorShader(L"Shader/ForwardPureColorShader.fx", 
			L"Shader/ForwardPureColorShader.fx"));

	mDepthGetShader = shared_ptr<Shader>(new Shader(L"Shader/DepthGetShader.fx",
		L"Shader/DepthGetShader.fx"));


	return true;
}


bool ShaderManager::SetDiffuseShader(CXMMATRIX worldMatrix,ID3D11ShaderResourceView* diffsuseTexture)
{
	bool result;

	result = mDiffuseShader->SetShaderParamsExtern(worldMatrix, diffsuseTexture);
	if (!result)
	{
		MessageBox(NULL, L"DiffuseShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L"DiffuseNormalShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L"DiffuseNormalSpecShader render  ß∞‹", NULL, MB_OK);
		return false;
	}

	return true;
}

bool ShaderManager::SetDefferLighingShader(ID3D11ShaderResourceView* gBuffer[4])
{
	bool result;
	result = mDefferLightingShader->SetShaderParams(gBuffer);

	if (!result)
	{
		MessageBox(NULL, L"mDefferLightingShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L"mUIShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L" depth shader render  ß∞‹", NULL, MB_OK);
		return false;
	}

	return true;
}

ShaderManager* ShaderManager::GetInstance()
{
	if (mShaderManager == nullptr)
	{
		mShaderManager = shared_ptr<ShaderManager>(new ShaderManager());
	}

	return mShaderManager.get();
}


bool ShaderManager::SetPureColorShader(CXMMATRIX worldMatrix,  FXMVECTOR surfaceColor)
{

	bool result;
	result = mPureColorShader->SetShaderParamsExtern(worldMatrix, surfaceColor);
	if (!result)
	{
		MessageBox(NULL, L" mPureColorShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L" mForwardPureColorShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L"DiffuseNormalSpecShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L"mGraphcisBlitShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L"DOFShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L"mBlurShader render  ß∞‹", NULL, MB_OK);
		return false;
	}

	return true;
}

bool ShaderManager::SetSSRShader(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* diffuseTex,
	ID3D11ShaderResourceView* depthTex, XMFLOAT2 perspectiveValues)
{

	bool result;

	result = mSSRShader->SetShaderParamsExtern(worldMatrix, diffuseTex,depthTex,
		perspectiveValues);
	if (!result)
	{
		MessageBox(NULL, L"mSSRShader render  ß∞‹", NULL, MB_OK);
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
		MessageBox(NULL, L"DepthGetShader render  ß∞‹", NULL, MB_OK);
		return false;
	}

	return true;
}

shared_ptr<ShaderManager> ShaderManager::mShaderManager = nullptr;

