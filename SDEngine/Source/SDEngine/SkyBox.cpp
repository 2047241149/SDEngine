#include "SkyBox.h"
#include "Texture/Texture.h"
#include "GameObject/GameObject.h"
#include "Scene/MeshComponent.h"
#include "Shader/ShaderManager.h"
#include "Texture/TextureSamplerManager.h"
#include "Shader/GeometryBuffer.h"
#include "GameObject/Camera.h"

const float SKY_BOX_SPHERE_SCALE = 10.0f;

SkyBox::SkyBox(const string& file)
{
	Init(file);
}

SkyBox::SkyBox(ID3D11ShaderResourceView* inSrv):
	cubeSrv(inSrv)
{
}

SkyBox::~SkyBox()
{
}

bool SkyBox::Init(const string& file)
{
	skyBoxTexture = make_shared<Texture>(file);
	skyBoxGameObject = make_shared<GameObject>();
	shared_ptr<MeshComponent> cubeMesh = make_shared<MeshComponent>("Resource\\FBXModel\\Engine\\cube.fbx");
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
	XMMATRIX worldMatrix = XMMatrixTranslation(GCamera_deprecated->position.x, GCamera_deprecated->position.y, GCamera_deprecated->position.z);
	XMMATRIX scaleMatrix = XMMatrixScaling(SKY_BOX_SPHERE_SCALE, SKY_BOX_SPHERE_SCALE, SKY_BOX_SPHERE_SCALE);
	GShaderManager->skyBoxShader->SetMatrix("World", scaleMatrix * worldMatrix);
	GShaderManager->skyBoxShader->SetMatrix("View", GCamera_deprecated->GetViewMatrix());
	GShaderManager->skyBoxShader->SetMatrix("Proj", GCamera_deprecated->GetProjectionMatrix());
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