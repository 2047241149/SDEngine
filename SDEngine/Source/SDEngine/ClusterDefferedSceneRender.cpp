#include "ClusterDefferedSceneRender.h"
#include "SkyBox.h"
#include "Texture/RenderTexture.h"
#include "WindowInfo.h"
#include "Shader/GeometryBuffer.h"
#include "Shader/CascadeShadowMapManager.h"
#include "DebugTool/Debugwindow.h"
#include "Shader/SSAOManager.h"
#include "Shader/SSRGBuffer.h"
#include "Shader/DepthBufferRT.h"
#include "IrradianceCubeMap.h"
#include "Texture/RWRenderTexture.h"
#include "PrefilterCubeMap.h"
#include "GameObject/Quad.h"
#include "Shader/ShaderManager.h"
#include "GameObject/GameObjectManager.h"
#include "Texture/TextureSamplerManager.h"
#include "Texture/TextureManager.h"
#include "SceneManager.h"

// 1024/16 = 768/12, squrd tile
static const int CLUTER_SIZE_X = 16;
static const int CLUTER_SIZE_Y = 12;
static const int CLUTER_SIZE_Z = 24;
static const int CLUTER_SIZE_NUM = CLUTER_SIZE_X * CLUTER_SIZE_Y * CLUTER_SIZE_Z;

ClusterDefferedSceneRender::ClusterDefferedSceneRender()
{
	Init();
}

ClusterDefferedSceneRender::~ClusterDefferedSceneRender()
{
}

ClusterDefferedSceneRender::ClusterDefferedSceneRender(const ClusterDefferedSceneRender&other)
{

}

bool ClusterDefferedSceneRender::Init()
{
	mSrcRT = shared_ptr<RenderTexture>(
		new RenderTexture(GScreenWidth, GScreenHeight));

	mSSRRT = shared_ptr<RenderTexture>(
		new RenderTexture(GScreenWidth, GScreenHeight));

	mConvolutedBrdfRT = shared_ptr<RenderTexture>(
		new RenderTexture(512, 512));

	mCascadeShadowsManager = shared_ptr<CascadedShadowsManager>(new CascadedShadowsManager(SHADOW_MAP_SIZE));

	mGrayShadowMap = shared_ptr<RenderTexture>(new RenderTexture(GScreenWidth, GScreenHeight, TextureFormat::R32));

	mLightBufferPointLight = shared_ptr<RenderTexture>(new RenderTexture(GScreenWidth, GScreenHeight, TextureFormat::R32G32B32A32));

	mLightBuffer = shared_ptr<RenderTexture>(new RenderTexture(GScreenWidth, GScreenHeight, TextureFormat::R32G32B32A32));

	mGeometryBuffer = shared_ptr<GeometryBuffer>(new
		GeometryBuffer(GScreenWidth, GScreenHeight, SCREEN_FAR, SCREEN_NEAR));

	mQuad = shared_ptr<Quad>(new Quad());

	mDebugWindow = shared_ptr<DebugWindow>(new DebugWindow(GScreenWidth, GScreenHeight, 120, 120));

	mBackDepthBufferRT = shared_ptr<DepthBufferRT>(new DepthBufferRT(GScreenWidth, GScreenHeight));

	mSSRBuffer = shared_ptr<SSRGBuffer>(new
		SSRGBuffer(GScreenWidth, GScreenHeight, SCREEN_FAR, SCREEN_NEAR));

	ssaoManager = shared_ptr<SSAOManager>(new SSAOManager(GScreenWidth, GScreenHeight));
	WCHAR* cubeMapResPath = L"Resource/Texture/night.dds";
	skyBox = shared_ptr<SkyBox>(new SkyBox(cubeMapResPath));
	radianceCubeMap = shared_ptr<IrradianceCubeMap>(new IrradianceCubeMap(cubeMapResPath));

	prefliterCubeMap = shared_ptr<PrefliterCubeMap>(new PrefliterCubeMap(cubeMapResPath, 512, 512));
	mClusterLightRT = shared_ptr<RWRenderTexture>(new RWRenderTexture(GScreenWidth, GScreenHeight));

	tileSizes.x = CLUTER_SIZE_X;
	tileSizes.y = CLUTER_SIZE_Y;
	tileSizes.z = CLUTER_SIZE_Z;
	tileSizes.w = (float)GWindowInfo->GetScreenWidth() / (float)CLUTER_SIZE_X;

	clusterFactor.x = (float)CLUTER_SIZE_Z / FMath::Log2(GCamera->GetFarPlane() / GCamera->GetNearPlane());
	clusterFactor.y = -((float)CLUTER_SIZE_Z *  FMath::Log2(GCamera->GetNearPlane()))/ FMath::Log2(GCamera->GetFarPlane() / GCamera->GetNearPlane());
	return true;
}

void ClusterDefferedSceneRender::PreRender()
{
	PreRenderDiffuseIrradiance();
	PreRenderFiliterCubeMap();
	PreRenderConvolutedBRDF();
	PreBuildCluster();
}

void ClusterDefferedSceneRender::Tick(float deltaTime)
{
	mCascadeShadowsManager->Update();
}

void ClusterDefferedSceneRender::PreRenderDiffuseIrradiance()
{
	radianceCubeMap->Render();
}

void ClusterDefferedSceneRender::PreRenderConvolutedBRDF()
{
	mConvolutedBrdfRT->SetRenderTarget();
	GDirectxCore->TurnOffZBuffer();
	GShaderManager->convolutedBRDFShader->Apply();
	mQuad->Render();
	GDirectxCore->RecoverDefaultDSS();
	GDirectxCore->RecoverDefualtRS();
}

void ClusterDefferedSceneRender::PreRenderFiliterCubeMap()
{
	prefliterCubeMap->Render();
}

void ClusterDefferedSceneRender::Render()
{
	g_RenderMask->BeginEvent(L"BeginScene");
	GDirectxCore->BeginScene(0.3f, 0.0f, 1.0f, 1.0f);
	g_RenderMask->EndEvent();

	//Pre Render Pass
	g_RenderMask->BeginEvent(L"RenderPreZPass");
	RenderPreZPass();
	g_RenderMask->EndEvent();

	//draw opacity
	g_RenderMask->BeginEvent(L"RenderOpacity");
	RenderOpacity();
	g_RenderMask->EndEvent();

	//render scene back depth buffer
#if SSR
	g_RenderMask->BeginEvent(L"RenderSceneBackDepthBuffer");
	RenderSceneBackDepthBuffer();
	g_RenderMask->EndEvent();
#endif

	//draw transparent object(normal transparent object，SSR)
	g_RenderMask->BeginEvent(L"RenderTransparency");
	RenderTransparency();
	g_RenderMask->EndEvent();

	g_RenderMask->BeginEvent(L"RenderPostEffect");
	RenderPostEffectPass();
	g_RenderMask->EndEvent();

	g_RenderMask->BeginEvent(L"RenderSkyBox");
	RenderSkyBoxPass();
	g_RenderMask->EndEvent();

#if DEBUG_GBUFFER
	g_RenderMask->BeginEvent(L"DEBUG_GBUFFER");
	RenderDebugWindow();
	g_RenderMask->EndEvent();
#endif

	GDirectxCore->EndScene();
}

void ClusterDefferedSceneRender::RenderGeometryPass()
{
	mGeometryBuffer->SetRenderTarget(XMFLOAT3(0.0f, 0.0f, 0.5f));
	vector<shared_ptr<GameObject>> vecGameObject = GGameObjectManager->m_vecGameObject;
	GDirectxCore->TurnOnPreDepthDSS();

	for (int index = 0; index < (int)vecGameObject.size(); index++)
	{
		shared_ptr<GameObject> pGameObject = vecGameObject[index];
		if (!pGameObject->m_pMesh->bTransparent)
		{
			if (!pGameObject->m_pMesh->bReflect)
			{
				pGameObject->Render();
			}
			else
			{
				//GDirectxCore->TurnOnMaskReflectDSS();
				pGameObject->Render();
				//GDirectxCore->TurnOnZBuffer();
			}

		}
	}

	GDirectxCore->RecoverDefaultDSS();
}


void ClusterDefferedSceneRender::RenderLightingPass()
{
	RenderClusterLightCullPass();
	RenderCullUnvalidClusterPass();

	g_RenderMask->BeginEvent(L"ClusterPointLightPassCs");
	if (bClusterUseCs)
	{
		RenderClusterPointLightPassCs();
	}
	else
	{
		RenderClusterPointLightPassPs();
	}

	g_RenderMask->EndEvent();
	RenderDirLightPass();
	RenderFinalShadingPass();
}


void ClusterDefferedSceneRender::RenderPostEffectPass()
{
	g_RenderMask->BeginEvent(L"FXAA");
	GDirectxCore->SetBackBufferRender();
	GDirectxCore->SetDefualtViewPort();
	GDirectxCore->TurnOffZBuffer();
	GShaderManager->fxaaShader->SetTexture("inputTexture", mSrcRT->GetSRV());
	GShaderManager->fxaaShader->SetFloat2("rcpFrame", XMFLOAT2(1.0f / (float)GScreenWidth, 1.0f / (float)GScreenHeight));
	GShaderManager->fxaaShader->SetTextureSampler("anisotropicSampler", GTextureSamplerManager->GetTextureSampler(TextureSampler::Anisotropic));
	GShaderManager->fxaaShader->Apply();
	mQuad->Render();
	GDirectxCore->TurnOnZBuffer();
	g_RenderMask->EndEvent();
}

void ClusterDefferedSceneRender::RenderDebugWindow()
{
	GDirectxCore->SetBackBufferRender();
	GDirectxCore->SetDefualtViewPort();
	GDirectxCore->TurnOffZBuffer();

	GShaderManager->uiShader->SetMatrix("UIView", GCamera->GetUIViewMatrix());
	GShaderManager->uiShader->SetMatrix("UIOrtho", GCamera->GetUIOrthoMatrix());

	//diffuse
	GShaderManager->uiShader->SetTexture("ShaderTexture",
		mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse));
	GShaderManager->uiShader->Apply();
	mDebugWindow->Render(10, 600);

	//pos
	GShaderManager->uiShader->SetTexture("ShaderTexture",
		mGeometryBuffer->GetGBufferSRV(GBufferType::Pos));
	GShaderManager->uiShader->Apply();
	mDebugWindow->Render(130, 600);

	//normal
	GShaderManager->uiShader->SetTexture("ShaderTexture",
		mGeometryBuffer->GetGBufferSRV(GBufferType::Normal));
	GShaderManager->uiShader->Apply();
	mDebugWindow->Render(250, 600);

	//specular
	GShaderManager->uiShader->SetTexture("ShaderTexture",
		mGeometryBuffer->GetGBufferSRV(GBufferType::SpecularRoughMetal));
	GShaderManager->uiShader->Apply();
	mDebugWindow->Render(370, 600);

	//ShadowMap
	GShaderManager->uiShader->SetTexture("ShaderTexture",
		mGrayShadowMap->GetSRV());
	GShaderManager->uiShader->Apply();
	mDebugWindow->Render(610, 600);

	//SSAO
	GShaderManager->uiShader->SetTexture("ShaderTexture",
		ssaoManager->GetSsaoSRV());
	GShaderManager->uiShader->Apply();
	mDebugWindow->Render(730, 600);

	//Depth
	GShaderManager->depthDisplayShader->SetTexture("ShaderTexture",
		mGeometryBuffer->GetGBufferSRV(GBufferType::Depth));
	GShaderManager->depthDisplayShader->SetMatrix("UIView", GCamera->GetUIViewMatrix());
	GShaderManager->depthDisplayShader->SetMatrix("UIOrtho", GCamera->GetUIOrthoMatrix());
	GShaderManager->depthDisplayShader->SetFloat("farPlane", GCamera->mFarPlane);
	GShaderManager->depthDisplayShader->SetFloat("nearPlane", GCamera->mNearPlane);
	GShaderManager->depthDisplayShader->Apply();
	mDebugWindow->Render(490, 600);

	//PreCompiteBRDF
	GShaderManager->uiShader->SetTexture("ShaderTexture",
		mConvolutedBrdfRT->GetSRV());
	GShaderManager->uiShader->Apply();
	mDebugWindow->Render(850, 600);

	//TiledLight
	GShaderManager->uiShader->SetTexture("ShaderTexture",
		bClusterUseCs ? mClusterLightRT->GetSRV() : mLightBufferPointLight->GetSRV());
	GShaderManager->uiShader->Apply();
	mDebugWindow->Render(850, 600);

#if SSR
	GShaderManager->SetDepthShader
	(mBackDepthBufferRT->GetShaderResourceView());
	mDebugWindow->Render(610, 600);

	GShaderManager->SetUIShader
	(mSSRBuffer->GetGBufferSRV(SSRBufferType::VIEW_NORMAL));
	mDebugWindow->Render(10, 480);

	GShaderManager->SetUIShader
	(mSSRBuffer->GetGBufferSRV(SSRBufferType::VIEW_POS));
	mDebugWindow->Render(10, 360);
#endif

	GDirectxCore->TurnOnZBuffer();
}

void ClusterDefferedSceneRender::RenderSSRPass()
{
	//mSSRRT->SetRenderTarget();
	ID3D11RenderTargetView* backRTV = GDirectxCore->GetRTV();
	//靠模板緩存值來判斷
	g_pDeviceContext->OMSetRenderTargets(1, &backRTV, nullptr);
	GDirectxCore->SetDefualtViewPort();
	GDirectxCore->TurnOnAlphaBlend();
	GDirectxCore->TurnOffZBuffer();
	vector<shared_ptr<GameObject>> vecGameObject = GGameObjectManager->m_vecGameObject;
	GDirectxCore->TurnOnPreDepthDSS();

	for (int index = 0; index < (int)vecGameObject.size(); index++)
	{
		shared_ptr<GameObject> pGameObject = vecGameObject[index];
		if (pGameObject->m_pMesh->bTransparent && pGameObject->m_pMesh->bReflect)
		{
			XMMATRIX worldMatrix = pGameObject->GetWorldMatrix();
			XMMATRIX projMatrix = GCamera->GetProjectionMatrix();
			XMFLOAT4X4 projFloat4X4;
			XMStoreFloat4x4(&projFloat4X4, projMatrix);
			XMFLOAT2 perspectiveValues;
			perspectiveValues.x = projFloat4X4.m[3][2];
			perspectiveValues.y = -projFloat4X4.m[2][2];
			GShaderManager->ssrShader->SetMatrix("View", GCamera->GetViewMatrix());
			GShaderManager->ssrShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
			GShaderManager->ssrShader->SetFloat("farPlane", GCamera->mFarPlane);
			GShaderManager->ssrShader->SetFloat("nearPlane", GCamera->mNearPlane);
			GShaderManager->ssrShader->SetFloat2("perspectiveValues", perspectiveValues);
			GShaderManager->ssrShader->SetTexture("DiffuseTex", mSrcRT->GetSRV());
			GShaderManager->ssrShader->SetTexture("FrontDepthTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Depth));
			GShaderManager->ssrShader->SetTexture("BackDepthTex", mBackDepthBufferRT->GetShaderResourceView());
			GShaderManager->ssrShader->SetTexture("ViewPosTex", mSSRBuffer->GetGBufferSRV(SSRBufferType::VIEW_POS));
			GShaderManager->ssrShader->SetTexture("ViewNormalTex", mSSRBuffer->GetGBufferSRV(SSRBufferType::VIEW_NORMAL));
			GShaderManager->ssrShader->Apply();
			mQuad->Render();
		}
	}

	GDirectxCore->TurnOnZBuffer();
	GDirectxCore->TurnOffAlphaBlend();
}

void ClusterDefferedSceneRender::RenderOpacity()
{
	g_RenderMask->BeginEvent(L"RenderGeometryPass");
	RenderGeometryPass();
	g_RenderMask->EndEvent();

	g_RenderMask->BeginEvent(L"RenderShadowMapPass");
	RenderShadowMapPass();
	g_RenderMask->EndEvent();

	g_RenderMask->BeginEvent(L"RenderSSAOPass");
	RenderSSAOPass();
	g_RenderMask->EndEvent();

	g_RenderMask->BeginEvent(L"RenderLightingPass");
	RenderLightingPass();
	g_RenderMask->EndEvent();
}

//绘制透明物体分为绘制透明
void ClusterDefferedSceneRender::RenderTransparency()
{
	RenderGeneralTransparency();

#if SSR
	RenderSSR();
#endif
}

void ClusterDefferedSceneRender::RenderGeneralTransparency()
{
	ID3D11RenderTargetView* backRTV = GDirectxCore->GetRTV();
	ID3D11DepthStencilView* opacityDSV = mGeometryBuffer->GetDSV();
	g_pDeviceContext->OMSetRenderTargets(1, &backRTV, opacityDSV);
	GDirectxCore->SetDefualtViewPort();
	GDirectxCore->TurnOnDisbleZWriteDSS();
	GDirectxCore->TurnOnAlphaBlend();

	vector<shared_ptr<GameObject>> vecGameObject = GGameObjectManager->m_vecGameObject;

	for (int index = 0; index < (int)vecGameObject.size(); index++)
	{
		shared_ptr<GameObject> pGameObject = vecGameObject[index];
		if (pGameObject->m_pMesh->bTransparent && !pGameObject->m_pMesh->bReflect)
		{
			/*XMMATRIX worldMatrix = pGameObject->GetWorldMatrix();
			//GShaderManager->(worldMatrix, XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));
			GShaderManager->mShader->SetMatrix("World", worldMatrix);
			GShaderManager->mShader->SetMatrix("View", GCamera->GetViewMatrix());
			GShaderManager->mShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
			GShaderManager->mShader->Apply();
			pGameObject->RenderMesh();*/
		}
	}

	GDirectxCore->TurnOffAlphaBlend();
	GDirectxCore->RecoverDefaultDSS();
}


void ClusterDefferedSceneRender::RenderSceneBackDepthBuffer()
{
	mBackDepthBufferRT->SetRenderTarget();
	GDirectxCore->TurnOnCullFront();
	vector<shared_ptr<GameObject>> vecGameObject = GGameObjectManager->m_vecGameObject;

	for (int index = 0; index < (int)vecGameObject.size(); index++)
	{
		shared_ptr<GameObject> pGameObject = vecGameObject[index];
		if (!pGameObject->m_pMesh->bTransparent)
		{
			pGameObject->Render(RenderMode::Simple);
		}
	}

	//恢复默认的RS
	GDirectxCore->RecoverDefualtRS();

}

void ClusterDefferedSceneRender::RenderSSRBufferPass()
{
	ID3D11DepthStencilView* backDSV = mGeometryBuffer->GetDSV();
	mSSRBuffer->SetRenderTarget(backDSV);
	GDirectxCore->TurnOnEnableReflectDSS();
	GShaderManager->ssrGBufferShader->SetTexture("WorldPosTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Pos));
	GShaderManager->ssrGBufferShader->SetTexture("WorldNormalTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Normal));
	GShaderManager->ssrGBufferShader->SetMatrix("View", GCamera->GetViewMatrix());
	GShaderManager->ssrGBufferShader->Apply();
	mQuad->Render();
	GDirectxCore->RecoverDefaultDSS();
	GDirectxCore->SetBackBufferRender();
}

void  ClusterDefferedSceneRender::RenderSSR()
{
	RenderSSRBufferPass();
	RenderSSRPass();
}

void ClusterDefferedSceneRender::ClearRwStrcutData()
{
	//GShaderManager->clearClusterDataCS->SetRWStructBufferInData("globalIndexCount", nullptr, 1);
	//GShaderManager->clearClusterDataCS->Dispatch(1, 1, 1);

	/*ID3D11Buffer* debugbuf = nullptr;
	ID3D11Buffer* resultBuffer = GShaderManager->clearClusterDataCS->GetBufferOfUav("globalIndexCount");
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	resultBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (FAILED(g_pDevice->CreateBuffer(&desc, nullptr, &debugbuf)))
	{
		return;
	}

	g_pDeviceContext->CopyResource(debugbuf, resultBuffer);
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	UINT8* pData;
	g_pDeviceContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);

	pData = (UINT8*)MappedResource.pData;

	UINT8 a = pData[0];

	for (int index = 0; index < 1; ++index)
	{
		std::cout << "  test=  " << (int)pData[index] << "     "<< std::endl;

	}

	g_pDeviceContext->Unmap(debugbuf, 0);*/
}

void ClusterDefferedSceneRender::RenderClusterLightCullPass()
{
	vector<PointLightParams> pointLights;
	GLightManager->GetPointLights(pointLights);
	if (pointLights.size() <= 0)
		return;

	ID3D11UnorderedAccessView* lightGridListUav = GShaderManager->buildClusterCS->GetUav("ClusterList");
	ID3D11UnorderedAccessView* ClusterActiveListUav = GShaderManager->maskUnvalidClusterCs->GetUav("ClusterActiveList");
	GShaderManager->clusterLightCullCS->SetStructBuffer("PointLights", pointLights.data(), pointLights.size());
	GShaderManager->clusterLightCullCS->SetFloat("lightCount", pointLights.size());
	GShaderManager->clusterLightCullCS->SetFloat("ScreenWidth", GCamera->mScreenWidth);
	GShaderManager->clusterLightCullCS->SetFloat("ScreenHeight", GCamera->mScreenHeight);
	GShaderManager->clusterLightCullCS->SetRWStructBuffer("ClusterList", lightGridListUav);
	GShaderManager->clusterLightCullCS->SetRWStructBuffer("ClusterActiveList", ClusterActiveListUav);
	GShaderManager->clusterLightCullCS->SetRWStructBufferInData("LightGridList", nullptr, CLUTER_SIZE_NUM);
	GShaderManager->clusterLightCullCS->SetRWStructBufferInData("GlobalLightIndexList", nullptr, 2097152);

	UINT clearData[4] = {0, 0, 0, 0};
	GShaderManager->clusterLightCullCS->SetRWStructBufferInData("globalIndexCount", clearData, 1);
	ID3D11UnorderedAccessView* globalIndexCountUav = GShaderManager->clusterLightCullCS->GetUav("globalIndexCount");
	if (globalIndexCountUav)
	{
		g_pDeviceContext->ClearUnorderedAccessViewUint(globalIndexCountUav, clearData);
	}

	GShaderManager->clusterLightCullCS->SetMatrix("View", GCamera->GetViewMatrix());
	GShaderManager->clusterLightCullCS->SetMatrix("ProjInv", FMath::GetInvense(GCamera->GetProjectionMatrix()));
	GShaderManager->clusterLightCullCS->Dispatch(1, 1, CLUTER_SIZE_Z / 2);
}

void ClusterDefferedSceneRender::RenderClusterPointLightPassPs()
{
	vector<PointLightParams> pointLights;
	GLightManager->GetPointLights(pointLights);
	if (pointLights.size() <= 0)
		return;

	ID3D11ShaderResourceView* shaderResourceView[4];
	shaderResourceView[0] = mGeometryBuffer->GetGBufferSRV(GBufferType::Pos);
	shaderResourceView[1] = mGeometryBuffer->GetGBufferSRV(GBufferType::Normal);
	shaderResourceView[2] = mGeometryBuffer->GetGBufferSRV(GBufferType::SpecularRoughMetal);
	shaderResourceView[3] = mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse);
	ID3D11UnorderedAccessView* LightGridListUav = GShaderManager->clusterLightCullCS->GetUav("LightGridList");
	ID3D11UnorderedAccessView* GlobalLightIndexListUav = GShaderManager->clusterLightCullCS->GetUav("GlobalLightIndexList");

	mLightBufferPointLight->SetRenderTarget(0.0f, 0.0f, 0.0f, 0.0f);
	GDirectxCore->TurnOffZBuffer();
	GShaderManager->clusterDefferedLightShader->SetTexture("WorldPosTex", shaderResourceView[0]);
	GShaderManager->clusterDefferedLightShader->SetTexture("WorldNormalTex", shaderResourceView[1]);
	GShaderManager->clusterDefferedLightShader->SetTexture("SpecularRoughMetalTex", shaderResourceView[2]);
	GShaderManager->clusterDefferedLightShader->SetTexture("AlbedoTex", shaderResourceView[3]);
	GShaderManager->clusterDefferedLightShader->SetTexture("DepthTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Depth));
	GShaderManager->clusterDefferedLightShader->SetFloat("lightCount", pointLights.size());
	GShaderManager->clusterDefferedLightShader->SetFloat3("cameraPos", GCamera->GetPosition());
	GShaderManager->clusterDefferedLightShader->SetFloat("ScreenWidth", GWindowInfo->GetScreenWidth());
	GShaderManager->clusterDefferedLightShader->SetFloat("ScreenHeight", GWindowInfo->GetScreenHeight());
	GShaderManager->clusterDefferedLightShader->SetFloat("farPlane", GCamera->mFarPlane);
	GShaderManager->clusterDefferedLightShader->SetFloat("nearPlane", GCamera->mNearPlane);
	GShaderManager->clusterDefferedLightShader->SetFloat4("tileSizes", tileSizes);
	GShaderManager->clusterDefferedLightShader->SetFloat2("cluserFactor", clusterFactor);
	GShaderManager->clusterDefferedLightShader->SetRWStructBuffer("LightGridList", LightGridListUav);
	GShaderManager->clusterDefferedLightShader->SetRWStructBuffer("GlobalLightIndexList", GlobalLightIndexListUav);
	GShaderManager->clusterDefferedLightShader->SetRWStructBufferInData("PointLightList", pointLights.data(), pointLights.size());
	GShaderManager->clusterDefferedLightShader->SetTextureSampler("clampLinearSample", GTextureSamplerBilinearClamp);
	GShaderManager->clusterDefferedLightShader->Apply();
	mQuad->Render();
}

void ClusterDefferedSceneRender::RenderClusterPointLightPassCs()
{
	vector<PointLightParams> pointLights;
	GLightManager->GetPointLights(pointLights);
	if (pointLights.size() <= 0)
		return;

	ID3D11UnorderedAccessView* LightGridListUav = GShaderManager->clusterLightCullCS->GetUav("LightGridList");
	ID3D11UnorderedAccessView* GlobalLightIndexListUav = GShaderManager->clusterLightCullCS->GetUav("GlobalLightIndexList");
	
	GShaderManager->clusterDefferedLightCS->SetTexture("DepthTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Depth));
	GShaderManager->clusterDefferedLightCS->SetTexture("WorldPosTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Pos));
	GShaderManager->clusterDefferedLightCS->SetTexture("WorldNormalTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Normal));
	GShaderManager->clusterDefferedLightCS->SetTexture("SpecularRoughMetalTex", mGeometryBuffer->GetGBufferSRV(GBufferType::SpecularRoughMetal));
	GShaderManager->clusterDefferedLightCS->SetTexture("AlbedoTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse));
	GShaderManager->clusterDefferedLightCS->SetTextureSampler("clampLinearSample", GTextureSamplerBilinearClamp);
	GShaderManager->clusterDefferedLightCS->SetStructBuffer("PointLights", pointLights.data(), pointLights.size());
	GShaderManager->clusterDefferedLightCS->SetRWStructBuffer("LightGridList", LightGridListUav);
	GShaderManager->clusterDefferedLightCS->SetRWStructBuffer("GlobalLightIndexList", GlobalLightIndexListUav);
	GShaderManager->clusterDefferedLightCS->SetRWTexture("OutputTexture", mClusterLightRT->GetUAV());
	GShaderManager->clusterDefferedLightCS->SetFloat4("tileSizes", tileSizes);
	GShaderManager->clusterDefferedLightCS->SetFloat2("cluserFactor", clusterFactor);
	GShaderManager->clusterDefferedLightCS->SetFloat("farPlane", GCamera->mFarPlane);
	GShaderManager->clusterDefferedLightCS->SetFloat("nearPlane", GCamera->mNearPlane);
	GShaderManager->clusterDefferedLightCS->SetFloat3("cameraPos", GCamera->GetPosition());
	GShaderManager->clusterDefferedLightCS->SetFloat("ScreenWidth", GCamera->mScreenWidth);
	GShaderManager->clusterDefferedLightCS->SetFloat("ScreenHeight", GCamera->mScreenHeight);
	GShaderManager->clusterDefferedLightCS->SetFloat("bDebugLightCount", GSceneManager->bDebugLightCount ? 1.0 : 0.0);
	GShaderManager->clusterDefferedLightCS->SetMatrix("View", GCamera->GetViewMatrix());
	GShaderManager->clusterDefferedLightCS->SetMatrix("ProjInv", FMath::GetInvense(GCamera->GetProjectionMatrix()));
	GShaderManager->clusterDefferedLightCS->Dispatch(100, 100, 1);
}

void ClusterDefferedSceneRender::RenderDirLightPass()
{
	mLightBuffer->SetRenderTarget();
	ID3D11ShaderResourceView* shaderResourceView[6];
	shaderResourceView[0] = mGeometryBuffer->GetGBufferSRV(GBufferType::Pos);
	shaderResourceView[1] = mGeometryBuffer->GetGBufferSRV(GBufferType::Normal);
	shaderResourceView[2] = mGeometryBuffer->GetGBufferSRV(GBufferType::SpecularRoughMetal);
	shaderResourceView[3] = mGrayShadowMap->GetSRV();
	shaderResourceView[4] = ssaoManager->GetSsaoSRV();
	shaderResourceView[5] = mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse);
	GDirectxCore->TurnOffZBuffer();

	//TODO: 只存在一次IradianceMap渲染?
	for (int index = 0; index < (int)GLightManager->m_vecDirLight.size(); ++index)
	{
		shared_ptr<DirectionLight> pDirLight = GLightManager->m_vecDirLight[index];
		XMFLOAT3 lightCo = pDirLight->GetLightColor();
		XMFLOAT4 lightColor = XMFLOAT4(lightCo.x, lightCo.y, lightCo.z, pDirLight->GetLightIntensity());
		GShaderManager->defferedDirLightShader->SetTexture("WorldPosTex", shaderResourceView[0]);
		GShaderManager->defferedDirLightShader->SetTexture("WorldNormalTex", shaderResourceView[1]);
		GShaderManager->defferedDirLightShader->SetTexture("SpecularRoughMetalTex", shaderResourceView[2]);
		GShaderManager->defferedDirLightShader->SetTexture("DirLightShadowMap", shaderResourceView[3]);
		GShaderManager->defferedDirLightShader->SetTexture("SSAORT", shaderResourceView[4]);
		GShaderManager->defferedDirLightShader->SetTexture("AlbedoTex", shaderResourceView[5]);
		GShaderManager->defferedDirLightShader->SetTexture("IrradianceTex", radianceCubeMap->GetIrradianceSrv());
		GShaderManager->defferedDirLightShader->SetTexture("PrefliterCubeMap", prefliterCubeMap->GetPrefilterCubeMapSrv());
		GShaderManager->defferedDirLightShader->SetTexture("BrdfLut", mConvolutedBrdfRT->GetSRV());
		GShaderManager->defferedDirLightShader->SetTexture("LightBuffer", bClusterUseCs ? mClusterLightRT->GetSRV() : mLightBufferPointLight->GetSRV());
		GShaderManager->defferedDirLightShader->SetFloat3("cameraPos", GCamera->GetPosition());
		GShaderManager->defferedDirLightShader->SetFloat4("lightColor", lightColor);
		GShaderManager->defferedDirLightShader->SetFloat3("lightDir", pDirLight->GetLightDirection());
		GShaderManager->defferedDirLightShader->SetTextureSampler("clampLinearSample", GTextureSamplerBilinearClamp);
		GShaderManager->defferedDirLightShader->SetTextureSampler("TrilinearFliterClamp", GTrilinearFliterClamp);

		ID3D11ShaderResourceView* memSSAORT = nullptr;
		if (bSSAO)
		{
			memSSAORT = ssaoManager->GetSsaoSRV();
		}
		else
		{
			memSSAORT = GWhiteTexture;
		}
		GShaderManager->defferedDirLightShader->SetTexture("SSAORT", memSSAORT);
		GShaderManager->defferedDirLightShader->Apply();
		mQuad->Render();
	}

	GDirectxCore->RecoverDefaultDSS();
}

void ClusterDefferedSceneRender::RenderFinalShadingPass()
{
	ID3D11RenderTargetView* pSceneRTV = mSrcRT->GetRenderTargetView();
	ID3D11DepthStencilView* opacityDSV = mGeometryBuffer->GetDSV();
	GDirectxCore->TurnOffZBuffer();
	GDirectxCore->SetDefualtViewPort();
	g_pDeviceContext->OMSetRenderTargets(1, &pSceneRTV, opacityDSV);
	GShaderManager->defferedFinalShader->SetTexture("DiffuseTex", mLightBuffer->GetSRV());
	GShaderManager->defferedFinalShader->SetTextureSampler("clampLinearSample", GTextureSamplerBilinearClamp);
	GShaderManager->defferedFinalShader->Apply();
	mQuad->Render();

	GDirectxCore->RecoverDefaultDSS();
}

void ClusterDefferedSceneRender::RenderShadowMapPass()
{
	if (GLightManager->m_vecDirLight.size() <= 0)
		return;

	XMMATRIX lightViewMatrix = GLightManager->GetMainLight()->GetViewMatrix();

	//渲染需要投射阴影的物体到RT上
	//后面可以考虑用GeometryShader减少DrawCall
	mCascadeShadowsManager->ClearDepthBuffer();
	for (int nCascadeIndex = 0; nCascadeIndex < CASCADE_SHADOW_MAP_NUM; ++nCascadeIndex)
	{
		mCascadeShadowsManager->SetRenderTarget(nCascadeIndex);

		for (int index = 0; index < (int)GGameObjectManager->m_vecGameObject.size(); ++index)
		{
			shared_ptr<GameObject> memGo = GGameObjectManager->m_vecGameObject[index];
			if (memGo->m_pMesh && !memGo->m_pMesh->bTransparent)
			{
				if (memGo->m_pMesh->bCastShadow)
				{
					GShaderManager->depthShader->SetMatrix("World", memGo->GetWorldMatrix());
					GShaderManager->depthShader->SetMatrix("View", lightViewMatrix);
					GShaderManager->depthShader->SetMatrix("Proj", mCascadeShadowsManager->mArrayLightOrthoMatrix[nCascadeIndex]);
					GShaderManager->depthShader->Apply();
					memGo->RenderMesh();
				}
			}
		}
	}

	//渲染得到阴影
	mGrayShadowMap->SetRenderTarget();
	GDirectxCore->TurnOffZBuffer();
	GShaderManager->shadowMapShader->SetTexture("WorldPosTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Pos));
	GShaderManager->shadowMapShader->SetTexture("CascadeLightDepthMap", mCascadeShadowsManager->GetShadowMapSRV());
	GShaderManager->shadowMapShader->SetMatrix("dirView", lightViewMatrix);
	for (int nCascadeIndex = 0; nCascadeIndex < CASCADE_SHADOW_MAP_NUM; ++nCascadeIndex)
	{
		GShaderManager->shadowMapShader->SetMatrixArrayElement("arrayDirProj",
			mCascadeShadowsManager->mArrayLightOrthoMatrix[nCascadeIndex], nCascadeIndex);
	}

	GShaderManager->shadowMapShader->SetFloat("shadowBias", 0.003f);
	GShaderManager->shadowMapShader->Apply();
	mQuad->Render();

	GDirectxCore->RecoverDefaultDSS();
}

void ClusterDefferedSceneRender::RenderSSAOPass()
{
	//渲染得到SSAORT
	ssaoManager->Render(mGeometryBuffer.get());
}

void ClusterDefferedSceneRender::RenderSkyBoxPass()
{
	skyBox->Render(mGeometryBuffer.get());
}
 
void ClusterDefferedSceneRender::RenderPreZPass()
{
	mGeometryBuffer->SetDepthTarget();
	vector<shared_ptr<GameObject>> vecGameObject = GGameObjectManager->m_vecGameObject;

	for (int index = 0; index < (int)vecGameObject.size(); index++)
	{
		shared_ptr<GameObject> pGameObject = vecGameObject[index];
		if (!pGameObject->m_pMesh->bTransparent)
		{
			pGameObject->Render(RenderMode::Simple);
		}
	}

	//恢复默认的RS
	GDirectxCore->RecoverDefualtRS();
}

void ClusterDefferedSceneRender::PreBuildCluster()
{
	GShaderManager->buildClusterCS->SetFloat("farPlane", GCamera->mFarPlane);
	GShaderManager->buildClusterCS->SetFloat("nearPlane", GCamera->mNearPlane);
	GShaderManager->buildClusterCS->SetFloat("ScreenWidth", GCamera->mScreenWidth);
	GShaderManager->buildClusterCS->SetFloat("ScreenHeight", GCamera->mScreenHeight);
	GShaderManager->buildClusterCS->SetFloat4("tileSizes", tileSizes);
	GShaderManager->buildClusterCS->SetRWStructBufferInData("ClusterList", nullptr, CLUTER_SIZE_X * CLUTER_SIZE_Y * CLUTER_SIZE_Z);
	GShaderManager->buildClusterCS->SetMatrix("View", GCamera->GetViewMatrix());
	GShaderManager->buildClusterCS->SetMatrix("ProjInv", FMath::GetInvense(GCamera->GetProjectionMatrix()));
	GShaderManager->buildClusterCS->Dispatch(CLUTER_SIZE_X, CLUTER_SIZE_Y, CLUTER_SIZE_Z);
}

void ClusterDefferedSceneRender::RenderCullUnvalidClusterPass()
{
	GShaderManager->maskUnvalidClusterCs->SetTexture("DepthTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Depth));
	GShaderManager->maskUnvalidClusterCs->SetTextureSampler("clampLinearSample", GTextureSamplerBilinearClamp);
	GShaderManager->maskUnvalidClusterCs->SetRWStructBufferInData("ClusterActiveList", nullptr, CLUTER_SIZE_NUM);
	GShaderManager->maskUnvalidClusterCs->SetFloat4("tileSizes", tileSizes);
	GShaderManager->maskUnvalidClusterCs->SetFloat2("cluserFactor", clusterFactor);
	GShaderManager->maskUnvalidClusterCs->SetFloat("farPlane", GCamera->mFarPlane);
	GShaderManager->maskUnvalidClusterCs->SetFloat("nearPlane", GCamera->mNearPlane);
	GShaderManager->maskUnvalidClusterCs->SetFloat("ScreenWidth", GCamera->mScreenWidth);
	GShaderManager->maskUnvalidClusterCs->SetFloat("ScreenHeight", GCamera->mScreenHeight);
	GShaderManager->maskUnvalidClusterCs->Dispatch(100, 100, 1);
}