
#ifndef TEXTURE_2D_DX_H
#define TEXTURE_2D_DX_H


#include "texture_dx.h"

namespace modelViewer::render::dx {
	class texture_2D_dx : public texture_dx {
	public:
		texture_2D_dx(texture_setup& texture_setup, Microsoft::WRL::ComPtr<ID3D12Device>& device,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);
		~texture_2D_dx();
		shader_texture_type getType() const override;
	protected:
		D3D12_SRV_DIMENSION getSRVDimension() const override;
		D3D12_RESOURCE_DIMENSION getResourceDimension() const override;
		int getArraySize() const override;
	};
}

#endif //TEXTURE_2D_DX_H
