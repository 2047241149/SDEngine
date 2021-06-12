#include "TiledDefferedSceneRender.h"
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
#include "Light/DirectionLight.h"
#include "Light/LightManager.h"


TiledDefferedSceneRender::TiledDefferedSceneRender()
{
	Init();
}

TiledDefferedSceneRender::~TiledDefferedSceneRender()
{
}

TiledDefferedSceneRender::TiledDefferedSceneRender(const TiledDefferedSceneRender&other)
{

}

bool TiledDefferedSceneRender::Init()
{
	mSrcRT = shared_ptr<RenderTexture>(
		new RenderTexture(GScreenWidth, GScreenHeight));

	mSSRRT = shared_ptr<RenderTexture>(
		new RenderTexture(GScreenWidth, GScreenHeight));

	mConvolutedBrdfRT = shared_ptr<RenderTexture>(
		new RenderTexture(512, 512));

	mCascadeShadowsManager = shared_ptr<CascadedShadowsManager>(new CascadedShadowsManager(SHADOW_MAP_SIZE));

	mGrayShadowMap = shared_ptr<RenderTexture>(new RenderTexture(GScreenWidth, GScreenHeight, TextureFormat::R32));

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
	mTiledLightRT = shared_ptr<RWRenderTexture>(new RWRenderTexture(GScreenWidth, GScreenHeight));
	return true;
}

void TiledDefferedSceneRender::PreRender()
{
	PreRenderDiffuseIrradiance();
	PreRenderFiliterCubeMap();
	PreRenderConvolutedBRDF();
}

void TiledDefferedSceneRender::Tick(float deltaTime)
{
	mCascadeShadowsManager->Update();
}

void TiledDefferedSceneRender::PreRenderDiffuseIrradiance()
{
	radianceCubeMap->Render();
}

void TiledDefferedSceneRender::PreRenderConvolutedBRDF()
{
	mConvolutedBrdfRT->SetRenderTarget();
	GDirectxCore->TurnOffZBuffer();
	GShaderManager->convolutedBRDFShader->Apply();
	mQuad->Render();
	GDirectxCore->RecoverDefaultDSS();
	GDirectxCore->RecoverDefualtRS();
}

void TiledDefferedSceneRender::PreRenderFiliterCubeMap()
{
	prefliterCubeMap->Render();
}

void TiledDefferedSceneRender::Render()
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

	//draw transparent object(normal transparent object��SSR)
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

void TiledDefferedSceneRender::RenderGeometryPass()
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


void TiledDefferedSceneRender::RenderLightingPass()
{
	RenderTiledLightPass();
	RenderDirLightPass();
	RenderFinalShadingPass();
}


void TiledDefferedSceneRender::RenderPostEffectPass()
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

void TiledDefferedSceneRender::RenderDebugWindow()
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
		mTiledLightRT->GetSRV());
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

void TiledDefferedSceneRender::RenderSSRPass()
{
	//mSSRRT->SetRenderTarget();
	ID3D11RenderTargetView* backRTV = GDirectxCore->GetRTV();
	//��ģ�徏��ֵ���Д�
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

void TiledDefferedSceneRender::RenderOpacity()
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

//����͸�������Ϊ����͸��
void TiledDefferedSceneRender::RenderTransparency()
{
	RenderGeneralTransparency();

#if SSR
	RenderSSR();
#endif
}

void TiledDefferedSceneRender::RenderGeneralTransparency()
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


void TiledDefferedSceneRender::RenderSceneBackDepthBuffer()
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

	//�ָ�Ĭ�ϵ�RS
	GDirectxCore->RecoverDefualtRS();

}

void TiledDefferedSceneRender::RenderSSRBufferPass()
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

void  TiledDefferedSceneRender::RenderSSR()
{
	RenderSSRBufferPass();
	RenderSSRPass();
}

void TiledDefferedSceneRender::RenderTiledLightPass()
{
	vector<PointLightParams> pointLights;
	GLightManager->GetPointLights(pointLights);
	if (pointLights.size() <= 0)
		return;

	GShaderManager->tiledLightShader->SetTexture("DepthTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Depth));
	GShaderManager->tiledLightShader->SetTexture("WorldPosTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Pos));
	GShaderManager->tiledLightShader->SetTexture("WorldNormalTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Normal));
	GShaderManager->tiledLightShader->SetTexture("SpecularRoughMetalTex", mGeometryBuffer->GetGBufferSRV(GBufferType::SpecularRoughMetal));
	GShaderManager->tiledLightShader->SetTexture("AlbedoTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse));
	GShaderManager->defferedPointLightShader->SetTextureSampler("clampLinearSample", GTextureSamplerBilinearClamp);
	GShaderManager->tiledLightShader->SetStructBuffer("PointLights", pointLights.data(), (int)pointLights.size());
	GShaderManager->tiledLightShader->SetRWTexture("OutputTexture", mTiledLightRT->GetUAV());
	GShaderManager->tiledLightShader->SetFloat("lightCount", (float)pointLights.size());
	GShaderManager->tiledLightShader->SetFloat("farPlane", GCamera->mFarPlane);
	GShaderManager->tiledLightShader->SetFloat("nearPlane", GCamera->mNearPlane);
	GShaderManager->tiledLightShader->SetFloat3("cameraPos", GCamera->GetPosition());
	GShaderManager->tiledLightShader->SetFloat("ScreenWidth", GCamera->mScreenWidth);
	GShaderManager->tiledLightShader->SetFloat("ScreenHeight", GCamera->mScreenHeight);
	GShaderManager->tiledLightShader->SetFloat("bDebugLightCount", GSceneManager->bDebugLightCount ? 1.0f : 0.0f);
	GShaderManager->tiledLightShader->SetMatrix("View", GCamera->GetViewMatrix());
	GShaderManager->tiledLightShader->SetMatrix("ProjInv", FMath::GetInvense(GCamera->GetProjectionMatrix()));
	GShaderManager->tiledLightShader->Dispatch(100, 100, 1);
}

void TiledDefferedSceneRender::RenderPointLightPass()
{
	ID3D11RenderTargetView* backRTV[1] = { nullptr };
	ID3D11DepthStencilView* opacityDSV = mGeometryBuffer->GetDSV();
	ID3D11RenderTargetView* pLightRTV = mLightBuffer->GetRenderTargetView();

	ID3D11ShaderResourceView* shaderResourceView[4];
	shaderResourceView[0] = mGeometryBuffer->GetGBufferSRV(GBufferType::Pos);
	shaderResourceView[1] = mGeometryBuffer->GetGBufferSRV(GBufferType::Normal);
	shaderResourceView[2] = mGeometryBuffer->GetGBufferSRV(GBufferType::SpecularRoughMetal);
	shaderResourceView[3] = mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse);

	for (int index = 0; index < (int)GLightManager->m_vecPointLight.size(); ++index)
	{
		g_pDeviceContext->OMSetRenderTargets(1, backRTV, opacityDSV);
		GDirectxCore->TurnOnMaskLightVolumeDSS();
		GDirectxCore->TurnOffFaceCull();
		shared_ptr<PointLight> pPoinntLight = GLightManager->m_vecPointLight[index];
		GShaderManager->forwardPureColorShader->SetMatrix("World", pPoinntLight->GetWorldMatrix());
		GShaderManager->forwardPureColorShader->SetMatrix("View", GCamera->GetViewMatrix());
		GShaderManager->forwardPureColorShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
		GShaderManager->forwardPureColorShader->SetMatrix("WorldInvTranspose", FMath::GetInvenseTranspose(pPoinntLight->GetWorldMatrix()));
		GShaderManager->forwardPureColorShader->Apply();
		//m_pPointVolume->RenderMesh();

		g_pDeviceContext->OMSetRenderTargets(1, &pLightRTV, opacityDSV);
		GDirectxCore->TurnOnLightBlend();
		GDirectxCore->TurnOnRenderLightVolumeDSS();
		shared_ptr<PointLight> pPointLight = GLightManager->m_vecPointLight[index];
		XMFLOAT3 lightCol = pPointLight->GetLightColor();
		XMFLOAT4 lightColor = XMFLOAT4(lightCol.x, lightCol.y, lightCol.z, pPointLight->GetLightIntensity());
		GShaderManager->defferedPointLightShader->SetTexture("WorldPosTex", shaderResourceView[0]);
		GShaderManager->defferedPointLightShader->SetTexture("WorldNormalTex", shaderResourceView[1]);
		GShaderManager->defferedPointLightShader->SetTexture("SpecularRoughMetalTex", shaderResourceView[2]);
		GShaderManager->defferedPointLightShader->SetTexture("AlbedoTex", shaderResourceView[3]);
		GShaderManager->defferedPointLightShader->SetMatrix("World", pPointLight->GetWorldMatrix());
		GShaderManager->defferedPointLightShader->SetMatrix("View", GCamera->GetViewMatrix());
		GShaderManager->defferedPointLightShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
		GShaderManager->defferedPointLightShader->SetFloat3("cameraPos", GCamera->GetPosition());
		GShaderManager->defferedPointLightShader->SetFloat4("lightColor", lightColor);
		GShaderManager->defferedPointLightShader->SetFloat3("lightPos", pPointLight->GetPosition());
		GShaderManager->defferedPointLightShader->SetFloat("radius", pPointLight->GetRadius());
		GShaderManager->defferedPointLightShader->SetFloat4("attenuation", pPointLight->GetLightAttenuation());
		GShaderManager->defferedPointLightShader->SetTextureSampler("clampLinearSample", GTextureSamplerBilinearClamp);
		GShaderManager->defferedPointLightShader->Apply();
		//m_pPointVolume->RenderMesh();
		GDirectxCore->TurnOffAlphaBlend();
	}

	GDirectxCore->RecoverDefualtRS();

}


void TiledDefferedSceneRender::RenderDirLightPass()
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
	GDirectxCore->TurnOnLightBlend();

	//TODO: ֻ����һ��IradianceMap��Ⱦ?
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
		GShaderManager->defferedDirLightShader->SetTexture("LightBuffer", mTiledLightRT->GetSRV());
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

	GDirectxCore->TurnOffAlphaBlend();
	GDirectxCore->RecoverDefaultDSS();
}

void TiledDefferedSceneRender::RenderFinalShadingPass()
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

void TiledDefferedSceneRender::RenderShadowMapPass()
{
	if (GLightManager->m_vecDirLight.size() <= 0)
		return;

	XMMATRIX lightViewMatrix = GLightManager->GetMainLight()->GetViewMatrix();

	//��Ⱦ��ҪͶ����Ӱ�����嵽RT��
	//������Կ�����GeometryShader����DrawCall
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

	//��Ⱦ�õ���Ӱ
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

void TiledDefferedSceneRender::RenderSSAOPass()
{
	//��Ⱦ�õ�SSAORT
	ssaoManager->Render(mGeometryBuffer.get());
}

void TiledDefferedSceneRender::RenderSkyBoxPass()
{
	skyBox->Render(mGeometryBuffer.get());
}

void TiledDefferedSceneRender::RenderPreZPass()
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

	//�ָ�Ĭ�ϵ�RS
	GDirectxCore->RecoverDefualtRS();
}