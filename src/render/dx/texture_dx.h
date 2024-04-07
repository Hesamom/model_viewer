
#ifndef TEXTURE_DX_H
#define TEXTURE_DX_H

#include <d3dx12.h>
#include <d3d12.h>
#include <wrl/client.h>
#include "../texture_setup.h"
#include "../texture.h"


namespace modelViewer::render::dx
{
	class texture_dx : public texture {
	public:
		virtual ~texture_dx() = default;
		D3D12_SHADER_RESOURCE_VIEW_DESC getView() const;
		ID3D12Resource* getResource() const;
		
	protected:
		void uploadData(modelViewer::render::texture_setup& texture_setup, Microsoft::WRL::ComPtr<ID3D12Device>& device,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);
		virtual D3D12_SRV_DIMENSION getSRVDimension() const = 0;
		virtual D3D12_RESOURCE_DIMENSION getResourceDimension() const = 0;
		virtual int getArraySize() const = 0;

		void createTextureFromRawData(texture_setup& setup,
			Microsoft::WRL::ComPtr<ID3D12Device>& comPtr,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& comPtr1);

		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_UploadHeap = nullptr;
		
	private:
		DXGI_FORMAT getTextureFormat(std::shared_ptr<modelViewer::res::texture_asset>& sharedPtr, bool compressed);
		int getBytesPerPixel(const DXGI_FORMAT& format);
	};
};


#endif //TEXTURE_DX_H
