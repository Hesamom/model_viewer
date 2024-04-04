
#ifndef TEXTURE_CUBE_DX_H
#define TEXTURE_CUBE_DX_H

#include "texture_dx.h"

namespace modelViewer::render::dx
{
	class texture_cube_dx : public texture_dx {
	public:
		texture_cube_dx(modelViewer::render::texture_setup& texture_setup, Microsoft::WRL::ComPtr<ID3D12Device>& device,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);
		
		shader_texture_type getType() const override;
	protected:
		int getArraySize() const override;
		D3D12_RESOURCE_DIMENSION getResourceDimension() const override;
		D3D12_SRV_DIMENSION getSRVDimension() const override;
	};
}

#endif //TEXTURE_CUBE_DX_H
