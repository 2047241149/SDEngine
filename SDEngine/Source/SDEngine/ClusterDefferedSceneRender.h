#pragma once
#ifndef _CLUTER_DEFFERED_SCENE_RENDER_H
#define _CLUTER_DEFFERED_SCENE_RENDER_H

class SkyBox;
class RenderTexture;
class Quad;
class SSAOManager;
class CascadedShadowsManager;
class SSRGBuffer;
class DebugWindow;
class GeometryBuffer;
class DepthBufferRT;
class Texture;
class GameObject;
class CubeCamera;
class RenderCubeMap;
class IrradianceCubeMap;
class PrefliterCubeMap;
class RWRenderTexture;
class SceneManager;

#include "SceneRender.h"
#include "Common/DirectxCore.h"
#include "Common/Macro.h"
#include"Light/Light.h"
#include"FPS.h"
#include<memory>
#include "Common/GraphicsConfig.h"
#include "SceneRender.h"
#include "GameObject/Mesh.h"


class ClusterDefferedSceneRender : public SceneRender
{
private:
	shared_ptr<Quad> mQuad;
	shared_ptr<Texture> whiteTexture;
	shared_ptr<RWRenderTexture> mClusterLightRT;

	//sky box
	shared_ptr<SkyBox> skyBox;
	shared_ptr<RenderTexture> mSSRRT;
	shared_ptr<RenderTexture> mSrcRT;
	shared_ptr<RenderTexture> mLightBufferPointLight;
	shared_ptr<RenderTexture> mLightBuffer;
	shared_ptr<RenderTexture> mGrayShadowMap;
	shared_ptr<CascadedShadowsManager> mCascadeShadowsManager;
	shared_ptr<SSAOManager> ssaoManager;

	//GeometryBuffer
	shared_ptr<GeometryBuffer> mGeometryBuffer;

	//PBR
	shared_ptr<RenderTexture> mConvolutedBrdfRT;
	shared_ptr<IrradianceCubeMap> radianceCubeMap;
	shared_ptr<PrefliterCubeMap> prefliterCubeMap;

	//SSRGBuffer
	shared_ptr<SSRGBuffer> mSSRBuffer;
	MaterialType materialType = MaterialType::DIFFUSE;
	MaterialType preMaterialType = MaterialType::DIFFUSE;
	shared_ptr<DebugWindow> mDebugWindow;
	shared_ptr<DepthBufferRT>  mBackDepthBufferRT;

	XMFLOAT2 clusterFactor;
	XMFLOAT4 tileSizes = XMFLOAT4(0.0, 0.0, 0.0, 0.0);
	bool bClusterUseCs = true;
	
public:
	ClusterDefferedSceneRender();
	ClusterDefferedSceneRender(const ClusterDefferedSceneRender&);
	~ClusterDefferedSceneRender();

private:
	void PreRenderDiffuseIrradiance();
	void PreRenderFiliterCubeMap();
	void PreRenderConvolutedBRDF();
	void PreBuildCluster();

private:
	void RenderPreZPass();
	void RenderOpacity();
	void RenderTransparency();
	void RenderGeneralTransparency();
	void RenderGeometryPass();
	void RenderLightingPass();
	void RenderFinalShadingPass();
	void RenderPostEffectPass();
	void RenderDebugWindow();
	void RenderSSRPass();
	void RenderSSAOPass();
	void RenderSSRBufferPass();
	void RenderSkyBoxPass();
	void RenderSceneBackDepthBuffer();
	void RenderClusterPointLightPassPs();
	void RenderClusterPointLightPassCs();
	void RenderDirLightPass();
	void RenderShadowMapPass();
	void RenderSSR();
	void RenderCullUnvalidClusterPass();
	void RenderClusterLightCullPass();
	void ClearRwStrcutData();

public:
	virtual bool Init() override;
	virtual void Tick(float deltaTime) override;
	virtual void PreRender() override;
	virtual void Render() override;
};

#endif // !_CLUTER_DEFFERED_SCENE_RENDER_H
