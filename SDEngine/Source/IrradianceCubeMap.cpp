#include "IrradianceCubeMap.h"
#include "GameObject/GameObject.h"
#include "GameObject/Mesh.h"
#include "Shader/ShaderManager.h"
#include "Texture/TextureSamplerManager.h"
#include "GameObject/Camera.h"
#include "GameObject/CubeCamera.h"
#include "Texture/HdrTexture.h"
#include "Texture/RenderCubeMap.h"

const float SKY_BOX_SPHERE_SCALE = 10.0f;

IrradianceCubeMap::IrradianceCubeMap(char* cubeMapFileName)
{
	Init(cubeMapFileName);
}

IrradianceCubeMap::IrradianceCubeMap(const IrradianceCubeMap& other)
{
}


IrradianceCubeMap::~IrradianceCubeMap()
{
}

bool IrradianceCubeMap::Init(char* cubeMapFileName)
{
	hdrTetxure = shared_ptr<HdrTexture>(new HdrTexture(cubeMapFileName));
	cubeGameObject = shared_ptr<GameObject>(new GameObject());
	shared_ptr<Mesh> cubeMesh = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\Engine\\cube.fbx"));
	cubeGameObject->SetMesh(cubeMesh);
	renderCubeMap = shared_ptr<RenderCubeMap>(new RenderCubeMap(512, 512));
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
		GShaderManager->equirectangularMapShader->SetMatrix("View", cubeCamera->GetViewMatrix(index));
		GShaderManager->equirectangularMapShader->SetMatrix("Proj", projMatrix);
		GShaderManager->equirectangularMapShader->SetTexture("EquirectangularMap", hdrTetxure->GetTexture());
		GShaderManager->equirectangularMapShader->SetTextureSampler("ClampLinear", GTextureSamplerBilinearClamp);
		GShaderManager->equirectangularMapShader->Apply();
		cubeGameObject->RenderMesh();
	}

	GDirectxCore->RecoverDefaultDSS();
	GDirectxCore->RecoverDefualtRS();
}

ID3D11ShaderResourceView* IrradianceCubeMap::GetSrv()
{
	return renderCubeMap->GetSRV();
}

ID3D11ShaderResourceView* IrradianceCubeMap::GetTexture2DSrv()
{
	return hdrTetxure->GetTexture();
}