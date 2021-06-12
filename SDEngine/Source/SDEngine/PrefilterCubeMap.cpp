#include "PrefilterCubeMap.h"
#include "GameObject/GameObject.h"
#include "GameObject/Mesh.h"
#include "Shader/ShaderManager.h"
#include "Texture/TextureSamplerManager.h"
#include "GameObject/Camera.h"
#include "GameObject/CubeCamera.h"
#include "Texture/HdrTexture.h"


PrefliterCubeMap::PrefliterCubeMap(WCHAR* cubeMapFileName, int textureWidth, int textureHeight)
{
	Init(cubeMapFileName, textureWidth, textureHeight);
}

PrefliterCubeMap::PrefliterCubeMap(const PrefliterCubeMap& other)
{
}


PrefliterCubeMap::~PrefliterCubeMap()
{
}

bool PrefliterCubeMap::Init(WCHAR* cubeMapFileName, int inTextureWidth, int inTextureHeight)
{
	hdrCubeMap = shared_ptr<Texture>(new Texture(cubeMapFileName));
	cubeGameObject = shared_ptr<GameObject>(new GameObject());
	shared_ptr<Mesh> cubeMesh = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\sphere\\sphere.fbx"));
	cubeGameObject->SetMesh(cubeMesh);
	textureWidth = inTextureWidth;
	textureHeight = inTextureHeight;
	cubeCamera = shared_ptr<CubeCamera>(new CubeCamera());
	cubeCamera->BuildCamera(XMFLOAT3(0.0f, 0.0f, 0.0f));
	return true;
}

void PrefliterCubeMap::ShutDown()
{
}

bool PrefliterCubeMap::Render()
{
	const int MaxMipLevel = 5;
	const int MaxCubeMapFaces = 6;
	ID3D11RenderTargetView* rtvs[MaxCubeMapFaces];
	const float color[4] = { 0.0, 0.0, 0.0, 1.0f };
	XMMATRIX projMatrix = cubeCamera->GetProjMatrix();
	//��һ,���2D�������ݽṹ��,������2D��ȾĿ������

	//Texture2D
	D3D11_TEXTURE2D_DESC cubeMapTextureDesc;
	ZeroMemory(&cubeMapTextureDesc, sizeof(cubeMapTextureDesc));
	cubeMapTextureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	cubeMapTextureDesc.Width = textureWidth;
	cubeMapTextureDesc.Height = textureHeight;
	cubeMapTextureDesc.MipLevels = 0;
	cubeMapTextureDesc.ArraySize = MaxCubeMapFaces;
	cubeMapTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	cubeMapTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	cubeMapTextureDesc.CPUAccessFlags = 0;
	cubeMapTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
	cubeMapTextureDesc.SampleDesc.Count = 1;
	cubeMapTextureDesc.SampleDesc.Quality = 0;
	HR(g_pDevice->CreateTexture2D(&cubeMapTextureDesc, NULL, &cubeMapTexture));

	//SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = cubeMapTextureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = MaxMipLevel; //-1�����ɵ�1X1���ص�MipMap
	HR(g_pDevice->CreateShaderResourceView(cubeMapTexture, &shaderResourceViewDesc, &srv));

	GDirectxCore->TurnOnRenderSkyBoxDSS();
	GDirectxCore->TurnOnCullFront();
	for (int mip = 0; mip < MaxMipLevel; ++mip)
	{
		D3D11_RENDER_TARGET_VIEW_DESC envMapRTVDesc;
		ZeroMemory(&envMapRTVDesc, sizeof(envMapRTVDesc));
		envMapRTVDesc.Format = cubeMapTextureDesc.Format;
		envMapRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		envMapRTVDesc.Texture2D.MipSlice = mip;
		envMapRTVDesc.Texture2DArray.ArraySize = 1;

		int mipWidth = int(textureWidth * pow(0.5, mip));
		int mipHeight = int(textureHeight * pow(0.5, mip));

		D3D11_VIEWPORT envMapviewport;
		envMapviewport.Width = (FLOAT)mipWidth;
		envMapviewport.Height = (FLOAT)mipHeight;
		envMapviewport.MinDepth = 0.0f;
		envMapviewport.MaxDepth = 1.0f;
		envMapviewport.TopLeftX = 0.0f;
		envMapviewport.TopLeftY = 0.0f;
		float roughness =(float)mip / (float)(MaxMipLevel - 1);

		for (int index = 0; index < MaxCubeMapFaces; ++index)
		{
			envMapRTVDesc.Texture2DArray.FirstArraySlice = index;
			g_pDevice->CreateRenderTargetView(cubeMapTexture, &envMapRTVDesc, &rtvs[index]);
			g_pDeviceContext->ClearRenderTargetView(rtvs[index], color);
			g_pDeviceContext->OMSetRenderTargets(1, &rtvs[index], 0);
			g_pDeviceContext->RSSetViewports(1, &envMapviewport);
			GShaderManager->prefilterCubeMapShader->SetMatrix("View", cubeCamera->GetViewMatrix(index));
			GShaderManager->prefilterCubeMapShader->SetMatrix("Proj", projMatrix);
			GShaderManager->prefilterCubeMapShader->SetFloat("Roughness", roughness);
			GShaderManager->prefilterCubeMapShader->SetTexture("HdrCubeMap", hdrCubeMap->GetTexture());
			GShaderManager->prefilterCubeMapShader->SetTextureSampler("TrilinearFliterClamp", GTrilinearFliterClamp);
			GShaderManager->prefilterCubeMapShader->Apply();
			cubeGameObject->RenderMesh();
		}

		for (int index = 0; index < MaxCubeMapFaces; ++index)
		{
			ReleaseCOM(rtvs[index]);
		}
	}

	GDirectxCore->RecoverDefaultDSS();
	GDirectxCore->RecoverDefualtRS();
	return true;
}

ID3D11ShaderResourceView* PrefliterCubeMap::GetPrefilterCubeMapSrv()
{
	return srv;
}

ID3D11ShaderResourceView* PrefliterCubeMap::GetCubeMapSrv()
{
	return hdrCubeMap->GetTexture();
}