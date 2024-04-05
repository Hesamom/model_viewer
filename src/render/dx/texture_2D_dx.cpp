#include "texture_2D_dx.h"
#include "../../common/string_util.h"


using namespace modelViewer::render::dx;

texture_2D_dx::texture_2D_dx(modelViewer::render::texture_setup& texture_setup,
	Microsoft::WRL::ComPtr<ID3D12Device>& device,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	if (texture_setup.type != res::texture_asset_type::texture2D)
	{
		throw std::runtime_error("Invalid texture setup type");
	}

	uploadData(texture_setup, device, commandList);
}


texture_2D_dx::~texture_2D_dx()
{

}

modelViewer::render::shader_texture_type texture_2D_dx::getType() const
{
	return modelViewer::render::shader_texture_type::texture2D;
}

D3D12_SRV_DIMENSION texture_2D_dx::getSRVDimension() const
{
	return D3D12_SRV_DIMENSION_TEXTURE2D;
}

D3D12_RESOURCE_DIMENSION texture_2D_dx::getResourceDimension() const
{
	return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
}

int texture_2D_dx::getArraySize() const
{
	return 1;
}

texture_2D_dx::texture_2D_dx(int width, int height,  std::string& name, Microsoft::WRL::ComPtr<ID3D12Device>& device)
{
	if (width <= 0 || height <= 0)
	{
		throw std::runtime_error("Invalid texture size");
	}
	
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC)); 
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; 
	texDesc.Alignment = 0; 
	texDesc.Width = width;
	texDesc.Height = height; 
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1; 
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_CLEAR_VALUE optClear;
	
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	
	device->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, &optClear,
		IID_PPV_ARGS(&m_Resource));
	
	auto wideName = ConvertAnsiToWide(name);
	m_Resource->SetName(wideName.data());
}
