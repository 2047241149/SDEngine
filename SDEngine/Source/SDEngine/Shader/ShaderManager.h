#pragma once
#ifndef _SHADER_MANAGER_H
#define _SHADER_MANAGER_H
#include "Shader.h"
#include<memory>
#include "../Common/Macro.h"
using namespace std;


class ShaderManager
{

public:
	shared_ptr<VertexPixelShader> diffuseShader;
	shared_ptr<VertexPixelShader> diffuseNormalSpecShader;
	shared_ptr<VertexPixelShader> diffuseNormalShader;
	shared_ptr<VertexPixelShader> diffuseSpecShader;
	shared_ptr<VertexPixelShader> uiShader;
	shared_ptr<VertexPixelShader> defferedDirLightShader;
	shared_ptr<VertexPixelShader> defferedPointLightShader;
	shared_ptr<VertexPixelShader> depthDisplayShader;
	shared_ptr<VertexPixelShader> pureColorShader;
	shared_ptr<VertexPixelShader> graphicsBlitShader;
	shared_ptr<VertexPixelShader> forwardPureColorShader;
	shared_ptr<VertexPixelShader> depthGetShader;
	shared_ptr<VertexPixelShader> ssrShader;
	shared_ptr<VertexPixelShader> ssrGBufferShader;
	shared_ptr<VertexPixelShader> waveShader;
	shared_ptr<VertexPixelShader> defferedFinalShader;
	shared_ptr<VertexPixelShader> fxaaShader;
	shared_ptr<VertexPixelShader> depthShader;
	shared_ptr<VertexPixelShader> shadowMapShader;
	shared_ptr<VertexPixelShader> ssaoShader;
	shared_ptr<VertexPixelShader> ssaoBlurShader;
	shared_ptr<VertexPixelShader> skyBoxShader;
	shared_ptr<VertexPixelShader> equirectangularMapShader;
	shared_ptr<VertexPixelShader> cubeMapToIrradianceShader;
	shared_ptr<VertexPixelShader> prefilterCubeMapShader;
	shared_ptr<VertexPixelShader> convolutedBRDFShader;
	shared_ptr<ComputeShader> tiledLightShader;
	shared_ptr<ComputeShader> buildClusterCS;
	shared_ptr<ComputeShader> clusterLightCullCS;
	shared_ptr<ComputeShader> clusterDefferedLightCS;
	shared_ptr<ComputeShader> maskUnvalidClusterCs;
	shared_ptr<VertexPixelShader> clusterDefferedLightShader;

private:
	static shared_ptr<ShaderManager> single;
	
public:
	ShaderManager();
	~ShaderManager();

	bool Init();
	static shared_ptr<ShaderManager> Get();
};
#endif 

#define GShaderManager (ShaderManager::Get())