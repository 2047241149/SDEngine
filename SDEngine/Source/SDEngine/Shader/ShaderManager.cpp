#include "ShaderManager.h"
#include "CascadeShadowMapManager.h"

ShaderManager::ShaderManager()
{
	Init();
}

ShaderManager::~ShaderManager()
{
}

bool ShaderManager::Init()
{
 	diffuseNormalShader = make_shared<VertexPixelShader>("Resource/Shader/DiffuseNormalShader.fx");

	diffuseNormalSpecShader = make_shared<VertexPixelShader>("Resource/Shader/DiffuseNormalSpecularShader.fx");

	diffuseSpecShader = make_shared<VertexPixelShader>("Resource/Shader/DiffuseSpecularShader.fx");

	diffuseShader = make_shared<VertexPixelShader>("Resource/Shader/DiffuseShader.fx");

	defferedDirLightShader = make_shared<VertexPixelShader>("Resource/Shader/DefferedDirLightShader.fx");

	defferedPointLightShader = make_shared<VertexPixelShader>("Resource/Shader/DefferedPointLightShader.fx");

	defferedFinalShader = make_shared<VertexPixelShader>("Resource/Shader/DefferedFinalShader.fx");

	depthDisplayShader = make_shared<VertexPixelShader>("Resource/Shader/DepthDisplayShader.fx");

	pureColorShader = make_shared<VertexPixelShader>("Resource/Shader/PureColorShader.fx");

	uiShader = make_shared<VertexPixelShader>("Resource/Shader/UIShader.fx");

	graphicsBlitShader = make_shared<VertexPixelShader>("Resource/Shader/GraphicsBlitShader.fx");
	
	ssrShader = make_shared<VertexPixelShader>("Resource/Shader/ScreenSpaceReflectShader.fx");

	forwardPureColorShader = make_shared<VertexPixelShader>("Resource/Shader/ForwardPureColorShader.fx");

	depthGetShader = make_shared<VertexPixelShader>("Resource/Shader/DepthGetShader.fx");

	ssrGBufferShader = make_shared<VertexPixelShader>("Resource/Shader/SSRGBuffer.fx");

	waveShader = make_shared<VertexPixelShader>("Resource/Shader/WaveShader.fx");

	fxaaShader = make_shared<VertexPixelShader>("Resource/Shader/FxaaShader.fx");

	depthShader = make_shared<VertexPixelShader>("Resource/Shader/LightDepthMap.fx");

	shadowMapShader = make_shared<VertexPixelShader>("Resource/Shader/DirShadowMapShader.fx");
	
	ssaoShader = make_shared<VertexPixelShader>("Resource/Shader/SSAOShader.hlsl");

	ssaoBlurShader = make_shared<VertexPixelShader>("Resource/Shader/SSAOBlurShader.hlsl");

	skyBoxShader = make_shared<VertexPixelShader>("Resource/Shader/SkyBoxShader.hlsl");

	equirectangularMapShader = make_shared<VertexPixelShader>("Resource/Shader/equirectangularMapShader.fx");

	cubeMapToIrradianceShader = make_shared<VertexPixelShader>("Resource/Shader/CubeMalpToIradiance.fx");

	prefilterCubeMapShader = make_shared<VertexPixelShader>("Resource/Shader/PreFilterHdrCubeMap.fx");

	convolutedBRDFShader = make_shared<VertexPixelShader>("Resource/Shader/ConvolutedBRDFShader.fx");

	tiledLightShader = make_shared<ComputeShader>("Resource/Shader/TiledLightDefferedShader.fx");
	
	buildClusterCS = make_shared<ComputeShader>("Resource/Shader/BuildClusterCS.fx");
	
	clusterLightCullCS = make_shared<ComputeShader>("Resource/Shader/ClusterLightCull.fx");
	
	clusterDefferedLightShader = make_shared<VertexPixelShader>("Resource/Shader/ClusterDefferedLightPixelShader.fx");

	clusterDefferedLightCS = make_shared<ComputeShader>("Resource/Shader/ClusterLightDefferedShader.fx");
	
	maskUnvalidClusterCs = make_shared<ComputeShader>("Resource/Shader/MaskUnvalidClusterCs.fx");
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