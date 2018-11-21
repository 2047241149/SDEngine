#pragma once
#ifndef _SHADER_MANAGER_H
#define _SHADER_MANAGER_H
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
	shared_ptr<Shader> depthDisplayShader;
	shared_ptr<Shader> pureColorShader;
	shared_ptr<Shader> graphicsBlitShader;
	shared_ptr<Shader> forwardPureColorShader;
	shared_ptr<Shader> depthGetShader;
	shared_ptr<Shader> ssrShader;
	shared_ptr<Shader> ssrGBufferShader;
	shared_ptr<Shader> waveShader;
	shared_ptr<Shader> defferedFinalShader;
	shared_ptr<Shader> fxaaShader;
	shared_ptr<Shader> lightDepthShader;
	shared_ptr<Shader> shadowMapShader;
	shared_ptr<Shader> ssaoShader;
	shared_ptr<Shader> ssaoBlurShader;
	shared_ptr<Shader> skyBoxShader;

private:
	static shared_ptr<ShaderManager> single;
	
public:
	ShaderManager();
	~ShaderManager();
	ShaderManager(const ShaderManager&);

	bool Init();
	static shared_ptr<ShaderManager> Get();
};
#endif 


#define GShaderManager (ShaderManager::Get())