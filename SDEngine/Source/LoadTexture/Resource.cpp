
#include "Resource.h"


HRESULT Resource::CreateShaderResourceViewFromFile(ID3D11Device* d3dDevice,
	const wchar_t* szFileName, ID3D11ShaderResourceView** textureView)
{
	if (!d3dDevice || !szFileName || !textureView)
		return E_INVALIDARG;

	HRESULT hr;
	WCHAR ext[_MAX_EXT];
	_wsplitpath_s(szFileName, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

	if (_wcsicmp(ext, L".dds") == 0)
	{
		hr = DirectX::CreateDDSTextureFromFile(d3dDevice, szFileName, nullptr, textureView);
	}
	else
	{
		hr = DirectX::CreateWICTextureFromFile(d3dDevice, nullptr, szFileName, nullptr, textureView);
	}

	return hr;
}