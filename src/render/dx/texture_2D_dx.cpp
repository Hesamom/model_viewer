#include "texture_2D_dx.h"


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

