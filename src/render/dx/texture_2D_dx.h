
#ifndef TEXTURE_2D_DX_H
#define TEXTURE_2D_DX_H


#include <d3d12.h>
#include <wrl/client.h>
#include "../texture_setup.h"
#include "../texture.h"

namespace modelViewer::render::dx {
	class texture_2D_dx : public texture {
	public:
		texture_2D_dx(texture_setup& texture_setup, Microsoft::WRL::ComPtr<ID3D12Device>& device,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);
		~texture_2D_dx();
		shader_texture_type getType() const override;
		D3D12_SHADER_RESOURCE_VIEW_DESC getView() const;
		ID3D12Resource* getResource() const;
		
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_UploadHeap = nullptr;

		void createTextureFromRawData(texture_setup& setup,
			Microsoft::WRL::ComPtr<ID3D12Device>& comPtr,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& comPtr1);

		DXGI_FORMAT getTextureFormat(std::shared_ptr<modelViewer::res::texture_asset>& sharedPtr, bool compressed);

		int getBytesPerPixel(const DXGI_FORMAT& format);
	};
}

#endif //TEXTURE_2D_DX_H
