
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

 	diffuseNormalShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/DiffuseNormalShader.fx", L"Resource/Shader/DiffuseNormalShader.fx"));

	diffuseNormalSpecShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/DiffuseNormalSpecularShader.fx", L"Resource/Shader/DiffuseNormalSpecularShader.fx"));

	diffuseSpecShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/DiffuseSpecularShader.fx", L"Resource/Shader/DiffuseSpecularShader.fx"));

	diffuseShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/DiffuseShader.fx", L"Resource/Shader/DiffuseShader.fx"));

	//创建以及初始化mPhongShader
	defferedDirLightShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/DefferedDirLightShader.fx",
			L"Resource/Shader/DefferedDirLightShader.fx"));

	defferedPointLightShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/DefferedPointLightShader.fx", L"Resource/Shader/DefferedPointLightShader.fx"));

	defferedFinalShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/DefferedFinalShader.fx", L"Resource/Shader/DefferedFinalShader.fx"));

	depthDisplayShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/DepthDisplayShader.fx", L"Resource/Shader/DepthDisplayShader.fx"));

	pureColorShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/PureColorShader.fx", L"Resource/Shader/PureColorShader.fx"));

	uiShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/UIShader.fx", L"Resource/Shader/UIShader.fx"));

	graphicsBlitShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/GraphicsBlitShader.fx", L"Resource/Shader/GraphicsBlitShader.fx"));
	
	ssrShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/ScreenSpaceReflectShader.fx", L"Resource/Shader/ScreenSpaceReflectShader.fx"));

	forwardPureColorShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/ForwardPureColorShader.fx",
			L"Resource/Shader/ForwardPureColorShader.fx"));

	depthGetShader = shared_ptr<VertexPixelShader>(new VertexPixelShader(L"Resource/Shader/DepthGetShader.fx",
		L"Resource/Shader/DepthGetShader.fx"));

	ssrGBufferShader = shared_ptr<VertexPixelShader>(new VertexPixelShader(L"Resource/Shader/SSRGBuffer.fx",
		L"Resource/Shader/SSRGBuffer.fx"));

	waveShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/WaveShader.fx", L"Resource/Shader/WaveShader.fx"));

	fxaaShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/FxaaShader.fx", L"Resource/Shader/FxaaShader.fx"));

	depthShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/LightDepthMap.fx", L"Resource/Shader/LightDepthMap.fx"));

	shadowMapShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/DirShadowMapShader.fx", L"Resource/Shader/DirShadowMapShader.fx"));
	
	ssaoShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/SSAOShader.hlsl", L"Resource/Shader/SSAOShader.hlsl"));

	ssaoBlurShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/SSAOBlurShader.hlsl", L"Resource/Shader/SSAOBlurShader.hlsl"));

	skyBoxShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/SkyBoxShader.hlsl", L"Resource/Shader/SkyBoxShader.hlsl"));

	equirectangularMapShader = shared_ptr<VertexPixelShader>(new
		VertexPixelShader(L"Resource/Shader/equirectangularMapShader.fx", L"Resource/Shader/equirectangularMapShader.fx"));

	cubeMapToIrradianceShader = shared_ptr<VertexPixelShader>(new VertexPixelShader(L"Resource/Shader/CubeMalpToIradiance.fx",
		L"Resource/Shader/CubeMalpToIradiance.fx"));

	prefilterCubeMapShader = shared_ptr<VertexPixelShader>(new VertexPixelShader(L"Resource/Shader/PreFilterHdrCubeMap.fx",
		L"Resource/Shader/PreFilterHdrCubeMap.fx"));

	convolutedBRDFShader = shared_ptr<VertexPixelShader>(new VertexPixelShader(L"Resource/Shader/ConvolutedBRDFShader.fx",
		L"Resource/Shader/ConvolutedBRDFShader.fx"));

	tiledLightShader = shared_ptr<ComputeShader>(new ComputeShader(L"Resource/Shader/TiledLightDefferedShader.fx"));
	return true;
}

shared_ptr<ShaderManager> ShaderManager::Get()
{
	if (nullptr == single)
	{
		single = shared_ptr<ShaderManager>(new ShaderManager());
	}

	return single;
}

shared_ptr<ShaderManager> ShaderManager::single = nullptr;