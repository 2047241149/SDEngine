#include "SSAOManager.h"
#include "RenderTexture.h"
#include "NoiseTexture.h"
#include <time.h>
#include "ShaderManager.h"
#include "Quad.h"
#include "TextureSamplerManager.h"
#include "GeometryBuffer.h"
#include "Camera.h"

const int SSAO_NOISE_TEXTURE_SIZE = 16;
const int SSAO_VEC_SCALE_NUM = 64;

SSAOManager::SSAOManager(int screenWidth, int screenHeight)
{
	Init(screenWidth, screenHeight);
}

SSAOManager::SSAOManager(const SSAOManager& other)
{

}

SSAOManager::~SSAOManager()
{

}

void SSAOManager::Init(int screenWidth, int screenHeight)
{
	ssaoRT = shared_ptr<RenderTexture>(new RenderTexture(screenWidth, screenHeight, TextureFormat::R32));

	ssaoDownSampleRT = shared_ptr<RenderTexture>(new RenderTexture((float)screenWidth / 2.0f, (float)screenHeight / 2.0f, TextureFormat::R32));

	ssaoDownSampleBlurRT = shared_ptr<RenderTexture>(new RenderTexture(screenWidth / 2.0f, screenHeight / 2.0f, TextureFormat::R32));

	ssaoUpSampleRT = shared_ptr<RenderTexture>(new RenderTexture(screenWidth, screenHeight, TextureFormat::R32));

	ssaoNoiseTexture = shared_ptr<NoiseTexture>(new NoiseTexture(SSAO_NOISE_TEXTURE_SIZE, SSAO_NOISE_TEXTURE_SIZE));

	quad = shared_ptr<Quad>(new Quad());

	vecSSAOSample.resize(SSAO_VEC_SCALE_NUM);

	for (int index = 0; index < SSAO_VEC_SCALE_NUM; ++index)
	{
		srand((unsigned)time(NULL));
		float randomX = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
		float randomY = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
		float randomZ = (float)rand() / (float)RAND_MAX;
		XMVECTOR random = XMVectorSet(randomX, randomY, randomZ, 0.0f);
		random = XMVector3Normalize(random);
		float randomFactor = (float)rand() / (float)RAND_MAX;
		random = XMVectorMultiply(random, XMVectorSet(randomFactor, randomFactor, randomFactor, 0.0f));
		float scale = float(index) / float(SSAO_VEC_SCALE_NUM);
		scale = FMath::lerp(0.1f, 1.0f, scale);
		random = XMVectorMultiply(random, XMVectorSet(scale, scale, scale, 0.0f));
		XMStoreFloat3(&vecSSAOSample[index], random);
	}

}

void SSAOManager::ShutDown()
{

}

void SSAOManager::Render(GeometryBuffer* geometryBuffer)
{
	if (nullptr == geometryBuffer)
		return;

	//渲染得到SSAORT
	ssaoRT->SetRenderTarget(1.0f, 1.0f, 1.0f, 1.0f);
	GDirectxCore->TurnOffZBuffer();
	GShaderManager->ssaoShader->SetTexture("WorldPosTex", geometryBuffer->GetGBufferSRV(GBufferType::Pos));
	GShaderManager->ssaoShader->SetTexture("WorldNormalTex", geometryBuffer->GetGBufferSRV(GBufferType::Normal));
	GShaderManager->ssaoShader->SetTexture("SSaoNoiseTexture", ssaoNoiseTexture->GetSRV());
	GShaderManager->ssaoShader->SetMatrix("View", GCamera->GetViewMatrix());
	GShaderManager->ssaoShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
	GShaderManager->ssaoShader->SetMatrix("ViewInvTranspose", FMath::GetInvenseTranspose(GCamera->GetViewMatrix()));
	for (int index = 0; index < SSAO_VEC_SCALE_NUM; ++index)
	{
		GShaderManager->ssaoShader->SetFloat3ArrayElement("ssaoSamples", vecSSAOSample[index], index);
	}
	GShaderManager->ssaoShader->SetTextureSampler("clampPointSample", GTextureSamplerPointClamp);
	GShaderManager->ssaoShader->SetTextureSampler("wrapPointSample", GTextureSamplerPointWrap);
	GShaderManager->ssaoShader->Apply();
	quad->Render();
	GDirectxCore->TurnOnZBuffer();

	//降采样得到SSAODownSampleRT
	ssaoDownSampleRT->SetRenderTarget();
	GDirectxCore->TurnOffZBuffer();
	GShaderManager->graphicsBlitShader->SetTexture("ScreenRT", ssaoRT->GetSRV());
	GShaderManager->graphicsBlitShader->SetTextureSampler("ClampLinear", GTextureSamplerBilinearClamp);
	GShaderManager->graphicsBlitShader->Apply();
	quad->Render();
	GDirectxCore->TurnOnZBuffer();

	//对SSAODownSampleRT进行模糊得到SSAODownSampleBlurRT
	ssaoDownSampleBlurRT->SetRenderTarget();
	GDirectxCore->TurnOffZBuffer();
	GShaderManager->ssaoBlurShader->SetTexture("ScreenRT", ssaoDownSampleRT->GetSRV());
	GShaderManager->ssaoBlurShader->SetTextureSampler("ClampLinear", GTextureSamplerBilinearClamp);
	GShaderManager->ssaoBlurShader->Apply();
	quad->Render();
	GDirectxCore->TurnOnZBuffer();

	//对SSAODownSampleBlurRT升采样
	ssaoUpSampleRT->SetRenderTarget();
	GDirectxCore->TurnOffZBuffer();
	GShaderManager->graphicsBlitShader->SetTexture("ScreenRT", ssaoDownSampleBlurRT->GetSRV());
	GShaderManager->graphicsBlitShader->SetTextureSampler("ClampLinear", GTextureSamplerBilinearClamp);
	GShaderManager->graphicsBlitShader->Apply();
	quad->Render();
	GDirectxCore->TurnOnZBuffer();
}

ID3D11ShaderResourceView* SSAOManager::GetSsaoSRV()
{
	return ssaoUpSampleRT->GetSRV();
}
