
#ifndef TEXTURE_2D_DX_H
#define TEXTURE_2D_DX_H


#include <d3d12.h>
#include <wrl/client.h>
#include "../texture_setup.h"

namespace modelViewer::render::dx {
	class texture_2D_dx {
	public:
		texture_2D_dx(texture_setup& texture_setup, Microsoft::WRL::ComPtr<ID3D12Device>& device,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);
		~texture_2D_dx();

		D3D12_SHADER_RESOURCE_VIEW_DESC getView() const;
		ID3D12Resource* getResource() const;
		
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_UploadHeap = nullptr;
	};
}

#endif //TEXTURE_2D_DX_H
