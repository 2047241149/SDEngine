#include "SkyBox.h"
#include "Texture.h"
#include "GameObject.h"
#include "Mesh.h"
#include "ShaderManager.h"
#include "TextureSamplerManager.h"
#include "GeometryBuffer.h"
#include "Camera.h"

const float SKY_BOX_SPHERE_SCALE = 10.0f;

SkyBox::SkyBox(WCHAR* cubeMapFileName)
{
	Init(cubeMapFileName);
}

SkyBox::SkyBox(const SkyBox& other)
{
}


SkyBox::~SkyBox()
{

}

bool SkyBox::Init(WCHAR* cubeMapFileName)
{
	skyBoxTexture = shared_ptr<Texture>(new Texture(cubeMapFileName));
	skyBoxGameObject = shared_ptr<GameObject>(new GameObject());
	shared_ptr<Mesh> cubeMesh = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\Engine\\cube.fbx"));
	skyBoxGameObject->SetMesh(cubeMesh);

	return true;
}

void SkyBox::ShutDown()
{

}

void SkyBox::Render(GeometryBuffer* geometryBuffer)
{
	if (nullptr == geometryBuffer)
		return;

	ID3D11RenderTargetView* backRTV = GDirectxCore->GetRTV();
	ID3D11DepthStencilView* opacityDSV = geometryBuffer->GetDSV();
	g_pDeviceContext->OMSetRenderTargets(1, &backRTV, opacityDSV);
	GDirectxCore->SetViewPort();
	GDirectxCore->TurnOnRenderSkyBoxDSS();
	GDirectxCore->TurnOnCullFront();
	XMMATRIX worldMatrix = XMMatrixTranslation(GCamera->mPosition.x, GCamera->mPosition.y, GCamera->mPosition.z);
	XMMATRIX scaleMatrix = XMMatrixScaling(SKY_BOX_SPHERE_SCALE, SKY_BOX_SPHERE_SCALE, SKY_BOX_SPHERE_SCALE);
	GShaderManager->skyBoxShader->SetMatrix("World", scaleMatrix * worldMatrix);
	GShaderManager->skyBoxShader->SetMatrix("View", GCamera->GetViewMatrix());
	GShaderManager->skyBoxShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
	GShaderManager->skyBoxShader->SetTexture("CubeMap", skyBoxTexture->GetTexture());
	GShaderManager->skyBoxShader->SetTextureSampler("ClampLinear", GTextureSamplerBilinearClamp);
	GShaderManager->skyBoxShader->Apply();
	skyBoxGameObject->RenderMesh();
	GDirectxCore->RecoverDefaultDSS();
	GDirectxCore->RecoverDefualtRS();
}