#ifndef _SHADOW_MAP_SHADER_H
#define _SHADOW_MAP_SHADER_H

#include "CoreMini.h"
#include"Camera.h"
#include "CommonConstantBufferStruct.h"
#include "2DShader.h"
#include "GraphicsConfig.h"
using namespace std;
using namespace DirectX;


class ShadowMapShader : public Shader_2D
{
private:
	struct CBShadowMap
	{
		XMMATRIX lightViewMatrix;
		XMMATRIX arrayLightProjMatrix[CASCADE_SHADOW_MAP_NUM];
		float lightCameraZ[3];
		float shadowBias;
	};
private:
	ID3D11Buffer* mCBCommon; 
	ID3D11Buffer* mCBShadowMap;
	ID3D11SamplerState* mClampPointSampler;

private:
	void CreateConstantBuffer();
	void CreateSampler();

protected:
	virtual void  ShutDown() override;
	virtual void SetShaderState() override;

public:
	ShadowMapShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	ShadowMapShader(const ShadowMapShader& other);
	virtual ~ShadowMapShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* worldPosTex, class CascadedShadowsManager* cascadedShadowsManager);
	bool SetShaderParams(ID3D11ShaderResourceView* worldPosTex, class CascadedShadowsManager* cascadedShadowsManager);
};
#endif 