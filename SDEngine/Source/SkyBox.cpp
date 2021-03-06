#include "SkyBox.h"
#include "Texture/Texture.h"
#include "GameObject/GameObject.h"
#include "GameObject/Mesh.h"
#include "Shader/ShaderManager.h"
#include "Texture/TextureSamplerManager.h"
#include "Shader/GeometryBuffer.h"
#include "GameObject/Camera.h"

const float SKY_BOX_SPHERE_SCALE = 10.0f;

SkyBox::SkyBox(WCHAR* cubeMapFileName)
{
	Init(cubeMapFileName);
}

SkyBox::SkyBox(const SkyBox& other)
{
}

SkyBox::SkyBox(ID3D11ShaderResourceView* inSrv):
	cubeSrv(inSrv)
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
	GDirectxCore->SetDefualtViewPort();
	GDirectxCore->TurnOnRenderSkyBoxDSS();
	GDirectxCore->TurnOnCullFront();
	XMMATRIX worldMatrix = XMMatrixTranslation(GCamera->mPosition.x, GCamera->mPosition.y, GCamera->mPosition.z);
	XMMATRIX scaleMatrix = XMMatrixScaling(SKY_BOX_SPHERE_SCALE, SKY_BOX_SPHERE_SCALE, SKY_BOX_SPHERE_SCALE);
	GShaderManager->skyBoxShader->SetMatrix("World", scaleMatrix * worldMatrix);
	GShaderManager->skyBoxShader->SetMatrix("View", GCamera->GetViewMatrix());
	GShaderManager->skyBoxShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
	ID3D11ShaderResourceView* srv = (nullptr != cubeSrv ? cubeSrv : skyBoxTexture->GetTexture());
	GShaderManager->skyBoxShader->SetTexture("CubeMap", srv);
	GShaderManager->skyBoxShader->SetTextureSampler("ClampLinear", GTextureSamplerBilinearClamp);
	GShaderManager->skyBoxShader->Apply();
	skyBoxGameObject->RenderMesh();
	GDirectxCore->RecoverDefaultDSS();
	GDirectxCore->RecoverDefualtRS();
}

void SkyBox::SetTexture(ID3D11ShaderResourceView* inSrv)
{
	cubeSrv = inSrv;
}