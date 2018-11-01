#pragma once
#ifndef _SHADER_MANAGER_H
#define _SHADER_MANAGER_H
#include "ScreenSpaceReflectShader.h"
#include "SSRGBufferShader.h"
#include "DefferedFinalShader.h"
#include "WaveShader.h"
#include "FxaaShader.h"
#include "LightDepthShader.h"
#include "ShadowMapShader.h"
#include "Shader.h"
#include<memory>
#include"Macro.h"
using namespace std;


class ShaderManager
{

public:
	shared_ptr<Shader> diffuseShader;
	shared_ptr<Shader> diffuseNormalSpecShader;
	shared_ptr<Shader> diffuseNormalShader;
	shared_ptr<Shader> diffuseSpecShader;
	shared_ptr<Shader> uiShader;
	shared_ptr<Shader> defferedDirLightShader;
	shared_ptr<Shader> defferedPointLightShader;
	shared_ptr<Shader> depthShader;
	shared_ptr<Shader> pureColorShader;
	shared_ptr<Shader> graphicsBlitShader;
	shared_ptr<Shader> forwardPureColorShader;
	shared_ptr<Shader> depthGetShader;
	shared_ptr<SSRShader> mSSRShader;
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

	bool SetSSRShader(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* arraySRV[5],
		XMFLOAT2 perspectiveValue);

	bool SetSSRGBufferShader(ID3D11ShaderResourceView* gBuffer[2]);

	bool SetWaveShader(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor, 
		ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal);

	bool SetDefferedFinalShader(ID3D11ShaderResourceView* gBuffer[2]);

	bool SetFXAAShader(ID3D11ShaderResourceView* screenRT, float fScreenWidth, float fScreenHeight);

	bool SetLightDepthShader(CXMMATRIX worldMatrix, CXMMATRIX lightOrthoProjMatrix);

	bool SetShadowMapShader(ID3D11ShaderResourceView* worldPosTex, class CascadedShadowsManager* cascadeShadowManager);
};
#endif 


#define GShaderManager (ShaderManager::Get())