#pragma once
#ifndef _SHADER_MANAGER_H
#define _SHADER_MANAGER_H
#include"DefferLightingShader.h"
#include "DiffuseShader.h"
#include"DiffuseNormalShader.h"
#include"DiffuseNormalSpecShader.h"
#include "UIShader.h"
#include"DepthShader.h"
#include "PureColorShader.h"
#include "DiffuseSpecShader.h"
#include "GraphicsBlitShader.h"
#include "DOFShader.h"
#include "BlurShader.h"
#include<memory>
#include"Macro.h"
using namespace std;


class ShaderManager
{

private:
	shared_ptr<DiffuseShader> mDiffuseShader;
	shared_ptr<UIShader> mUIShader;
	shared_ptr<DepthShader> mDepthShader;
	shared_ptr<PureColorShader> mPureColorShader;
	shared_ptr<DiffuseNormalShader> mDiffuseNormalShader;
	shared_ptr<DiffuseNormalSpecShader> mDiffuseNormalSpecShader;
	shared_ptr<DiffuseSpecShader> mDiffuseSpecShader;
	shared_ptr<DefferLighingShader> mDefferLightingShader;
	shared_ptr<GraphcisBlitShader> mGraphcisBlitShader;
	shared_ptr<DOFShader> mDOFShader;
	shared_ptr<BlurShader> mBlurShader;
public:
	static shared_ptr<ShaderManager> mShaderManager;

	static ShaderManager* GetInstance();
public:
	ShaderManager();
	~ShaderManager();
	ShaderManager(const ShaderManager&);

	bool Initialize();

	//对外的Render方法接口
	bool SetDiffuseShader(CXMMATRIX worldMatrix ,ID3D11ShaderResourceView* diffuseTexture);

	bool SetDiffuseNormalShader(CXMMATRIX worldMatrix,
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV);

	bool SetDiffuseNormalSpecShader(CXMMATRIX worldMatrix,
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV,
		ID3D11ShaderResourceView* specSRV);

	bool SetDefferLighingShader(ID3D11ShaderResourceView* gBuffer[4]);

	bool SetUIShader(ID3D11ShaderResourceView* diffuseTexture);


	bool SetDepthShader(CXMMATRIX uiViewMatrix, CXMMATRIX uiOrhoMatrix,
		ID3D11ShaderResourceView* depthTexture);

	bool SetPureColorShader(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor);

	bool SetDiffuseSpecShader(CXMMATRIX worldMatrix,
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* specSRV);

	bool SetGraphcisBlitShader(ID3D11ShaderResourceView* screenRT);

	bool SetDOFShader(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT,
		ID3D11ShaderResourceView* depthRT,
		float dofStart, float dofRange, float farPlane, float nearPlane);

	bool SetBlurShader(ID3D11ShaderResourceView* screenRT);

public:
	DepthShader* GetDepthShader() { return mDepthShader.get(); }
	DiffuseShader* GetDiffuseShader() { return mDiffuseShader.get(); }
	DiffuseNormalShader* GetDiffuseNormalShader() { return mDiffuseNormalShader.get(); }
	DiffuseNormalSpecShader* GetDiffuseNormalSpecShader() { return mDiffuseNormalSpecShader.get(); }
	DefferLighingShader* GetDefferLighingShader(){ return  mDefferLightingShader.get(); }
	UIShader* GetUIShader() { return mUIShader.get(); }
	PureColorShader* GetPureColorShader() { return mPureColorShader.get(); }
	DiffuseSpecShader* GetDiffuseSpecShader() { return mDiffuseSpecShader.get(); }
	GraphcisBlitShader* GetGraphcisBlitShader() { return mGraphcisBlitShader.get(); }
	DOFShader* GetDOFShader() { return mDOFShader.get(); }
	BlurShader* GetBlurShader() { return mBlurShader.get(); }
};
#endif // !_SHADER_MANAGER_CLASS

