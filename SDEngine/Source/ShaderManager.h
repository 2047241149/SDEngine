#pragma once
#ifndef _SHADER_MANAGER_H
#define _SHADER_MANAGER_H
#include "DefferedDirLightShader.h"
#include "DefferedPointLightShader.h"
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
#include "ScreenSpaceReflectShader.h"
#include "SSRGBufferShader.h"
#include "DefferedFinalShader.h"
#include "WaveShader.h"
#include "FxaaShader.h"
#include "LightDepthShader.h"
#include "ShadowMapShader.h"
#include<memory>
#include"Macro.h"
using namespace std;


class ShaderManager
{

public:
	shared_ptr<DiffuseShader> mDiffuseShader;
	shared_ptr<UIShader> mUIShader;
	shared_ptr<DepthShader> mDepthShader;
	shared_ptr<PureColorShader> mPureColorShader;
	shared_ptr<DiffuseNormalShader> mDiffuseNormalShader;
	shared_ptr<DiffuseNormalSpecShader> mDiffuseNormalSpecShader;
	shared_ptr<DiffuseSpecShader> mDiffuseSpecShader;
	shared_ptr<DefferedDirLightShader> mDefferDirLightShader;
	shared_ptr<DefferedPointLightShader> mDefferPointLightShader;
	shared_ptr<GraphcisBlitShader> mGraphcisBlitShader;
	shared_ptr<DOFShader> mDOFShader;
	shared_ptr<BlurShader> mBlurShader;
	shared_ptr<SSRShader> mSSRShader;
	shared_ptr<PureColorShader> mForwardPureColorShader;
	shared_ptr<Shader_3D> mDepthGetShader;
	shared_ptr<SSRGBufferShader> mSSRGBufferShader;
	shared_ptr<WaveShader> mWaveShader;
	shared_ptr<DefferedFinalShader> mDefferedFinalShader;
	shared_ptr<FxaaShader> mFxaaShader;
	shared_ptr<LightDepthShader> mLightDepthShader;
	shared_ptr<ShadowMapShader> mShadowMapShader;
private:
	static shared_ptr<ShaderManager> m_spShaderManager;
	
public:
	ShaderManager();
	~ShaderManager();
	ShaderManager(const ShaderManager&);

	bool Init();
	static shared_ptr<ShaderManager> Get();
	//对外的Render方法接口
	bool SetDiffuseShader(CXMMATRIX worldMatrix ,ID3D11ShaderResourceView* diffuseTexture);

	bool SetDiffuseNormalShader(CXMMATRIX worldMatrix,
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV);

	bool SetDiffuseNormalSpecShader(CXMMATRIX worldMatrix,
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV,
		ID3D11ShaderResourceView* specSRV);

	bool SetUIShader(ID3D11ShaderResourceView* diffuseTexture);


	bool SetDepthShader(ID3D11ShaderResourceView* depthTexture);

	bool SetPureColorShader(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor);

	bool SetForwardPureColorShader(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor);

	bool SetDiffuseSpecShader(CXMMATRIX worldMatrix,
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* specSRV);

	bool SetGraphcisBlitShader(ID3D11ShaderResourceView* screenRT);

	bool SetDOFShader(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT,
		ID3D11ShaderResourceView* depthRT,
		float dofStart, float dofRange, float farPlane, float nearPlane);

	bool SetBlurShader(ID3D11ShaderResourceView* screenRT);

	bool SetSSRShader(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* arraySRV[5],
		XMFLOAT2 perspectiveValue);

	bool SetDepthGetShader(CXMMATRIX worldMatrix);

	bool SetSSRGBufferShader(ID3D11ShaderResourceView* gBuffer[2]);

	bool SetWaveShader(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor, 
		ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal);

	bool SetDefferedDirLightShader(ID3D11ShaderResourceView* gBuffer[4], int nDirLightIndex);

	bool SetDefferedPointLightShader(ID3D11ShaderResourceView* gBuffer[3], int nPointLightIndex);

	bool SetDefferedFinalShader(ID3D11ShaderResourceView* gBuffer[2]);

	bool SetFXAAShader(ID3D11ShaderResourceView* screenRT, float fScreenWidth, float fScreenHeight);

	bool SetLightDepthShader(CXMMATRIX worldMatrix);

	bool SetShadowMapShader(ID3D11ShaderResourceView* worldPosTex, ID3D11ShaderResourceView*  lightDepthMap, int nDirLightIndex);
};
#endif 


#define GShaderManager (ShaderManager::Get())