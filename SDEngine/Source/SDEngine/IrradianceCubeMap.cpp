#include "IrradianceCubeMap.h"
#include "GameObject/GameObject.h"
#include "GameObject/Mesh.h"
#include "Shader/ShaderManager.h"
#include "Texture/TextureSamplerManager.h"
#include "GameObject/Camera.h"
#include "GameObject/CubeCamera.h"
#include "Texture/HdrTexture.h"
#include "Texture/RenderCubeMap.h"


IrradianceCubeMap::IrradianceCubeMap(const string& file, int textureWidth, int textureHeight)
{
	Init(file, textureWidth, textureHeight);
}

IrradianceCubeMap::IrradianceCubeMap(const IrradianceCubeMap& other)
{
}


IrradianceCubeMap::~IrradianceCubeMap()
{
}

bool IrradianceCubeMap::Init(const string& file, int textureWidth, int textureHeight)
{
	hdrCubeMap = shared_ptr<Texture>(new Texture(file));
	cubeGameObject = shared_ptr<GameObject>(new GameObject());
	shared_ptr<Mesh> cubeMesh = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\sphere\\sphere.fbx"));
	cubeGameObject->SetMesh(cubeMesh);
	renderCubeMap = shared_ptr<RenderCubeMap>(new RenderCubeMap(textureWidth, textureHeight));
	cubeCamera = shared_ptr<CubeCamera>(new CubeCamera());
	cubeCamera->BuildCamera(XMFLOAT3(0.0f, 0.0f, 0.0f));
	return true;
}

void IrradianceCubeMap::ShutDown()
{
}

void IrradianceCubeMap::Render()
{
	GDirectxCore->TurnOnRenderSkyBoxDSS();
	GDirectxCore->TurnOnCullFront();
	renderCubeMap->ClearRenderTarget(1.0f, 1.0f, 1.0f, 1.0f);
	XMMATRIX projMatrix = cubeCamera->GetProjMatrix();

	for (int index = 0; index < 6; ++index)
	{
		renderCubeMap->ClearDepthBuffer();
		renderCubeMap->SetRenderTarget(index);
		GShaderManager->cubeMapToIrradianceShader->SetMatrix("View", cubeCamera->GetViewMatrix(index));
		GShaderManager->cubeMapToIrradianceShader->SetMatrix("Proj", projMatrix);
		GShaderManager->cubeMapToIrradianceShader->SetTexture("HdrCubeMap", hdrCubeMap->GetTexture());
		GShaderManager->cubeMapToIrradianceShader->SetTextureSampler("ClampLinear", GTextureSamplerBilinearClamp);
		GShaderManager->cubeMapToIrradianceShader->Apply();
		cubeGameObject->RenderMesh();
	}

	//Gen MipMap
	GDirectxCore->GenerateMips(renderCubeMap->GetSRV());
	GDirectxCore->RecoverDefaultDSS();
	GDirectxCore->RecoverDefualtRS();
}

ID3D11ShaderResourceView* IrradianceCubeMap::GetIrradianceSrv()
{
	return renderCubeMap->GetSRV();
}

ID3D11ShaderResourceView* IrradianceCubeMap::GetCubeMapSrv()
{
	return hdrCubeMap->GetTexture();
}