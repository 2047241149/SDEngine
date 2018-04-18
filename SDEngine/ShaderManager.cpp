
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


	//创建以及初始化mPhongShader
	mDiffuseShader = shared_ptr<DiffuseShader>(new
		DiffuseShader(L"Shader/DiffuseShader.fx", L"Shader/DiffuseShader.fx"));

	//创建以及初始化mPhongShader
	mDefferLightingShader = shared_ptr<DefferLighingShader>(new 
		DefferLighingShader(L"Shader/DefferLightingShader.fx", L"Shader/DefferLightingShader.fx"));

	mDepthShader = shared_ptr<DepthShader>(new
		DepthShader(L"Shader/DepthShader.fx", L"Shader/DepthShader.fx"));

	mPureColorShader = shared_ptr<PureColorShader>(new
		PureColorShader(L"Shader/PureColorShader.fx", L"Shader/PureColorShader.fx"));

	mUIShader = shared_ptr<UIShader>(new
		UIShader(L"Shader/UIShader.fx", L"Shader/UIShader.fx"));

	return true;
}


bool ShaderManager::SetDiffuseShader(CXMMATRIX worldMatrix,ID3D11ShaderResourceView* diffsuseTexture)
{
	bool result;

	result = mDiffuseShader->SetShaderParamsExtern(worldMatrix, diffsuseTexture);
	if (!result)
	{
		MessageBox(NULL, L"DiffuseShader render 失败", NULL, MB_OK);
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
		MessageBox(NULL, L"DiffuseNormalShader render 失败", NULL, MB_OK);
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
		MessageBox(NULL, L"DiffuseNormalSpecShader render 失败", NULL, MB_OK);
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
		MessageBox(NULL, L"mDefferLightingShader render 失败", NULL, MB_OK);
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
		MessageBox(NULL, L"mUIShader render 失败", NULL, MB_OK);
		return false;
	}

	return true;
}



bool ShaderManager::SetDepthShader(CXMMATRIX uiViewMatrix, CXMMATRIX uiOrhoMatrix, ID3D11ShaderResourceView* depthTexture)
{
	bool result;
	result = mDepthShader->SetShaderParams(uiViewMatrix, uiOrhoMatrix, depthTexture);
	if (!result)
	{
		MessageBox(NULL, L" depth shader render 失败", NULL, MB_OK);
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
		MessageBox(NULL, L" mPureColorShader render 失败", NULL, MB_OK);
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
		MessageBox(NULL, L"DiffuseNormalSpecShader render 失败", NULL, MB_OK);
		return false;
	}

	return true;
}


shared_ptr<ShaderManager> ShaderManager::mShaderManager = nullptr;

