#include "DirectxFrame.h"

namespace DirectXFrame
{
	HRESULT CreateRasterizerState(ID3D11Device* pDevice, ID3D11RasterizerState** ppRasterizerState, D3D11_CULL_MODE CullMode,
		D3D11_FILL_MODE FillMode, bool bDepthClipEnable)
	{
		
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = CullMode; //�����޳�
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = bDepthClipEnable; //��Ȳü�����
		rasterDesc.FillMode = FillMode; //ʵ����Ⱦ
		rasterDesc.FrontCounterClockwise = false; //˳ʱ��
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;
		return pDevice->CreateRasterizerState(&rasterDesc, ppRasterizerState);
	}
	
}