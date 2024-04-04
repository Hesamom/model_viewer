
#include "texture_cube_dx.h"
using namespace modelViewer::render;
using namespace modelViewer::render::dx;


texture_cube_dx::texture_cube_dx(texture_setup& texture_setup,
	Microsoft::WRL::ComPtr<ID3D12Device>& device,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	if (texture_setup.type != res::texture_asset_type::textureCube)
	{
		throw std::runtime_error("Invalid texture setup type");
	}
	
	assert(texture_setup.assets.size() == 6);
	uploadData(texture_setup, device, commandList);
}

shader_texture_type texture_cube_dx::getType() const
{
	return shader_texture_type::textureCube;
}

int texture_cube_dx::getArraySize() const
{
	return 6;
}

D3D12_RESOURCE_DIMENSION texture_cube_dx::getResourceDimension() const
{
	return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
}

D3D12_SRV_DIMENSION texture_cube_dx::getSRVDimension() const
{
	return D3D12_SRV_DIMENSION_TEXTURECUBE;
}

