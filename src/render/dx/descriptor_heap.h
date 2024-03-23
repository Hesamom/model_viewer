
#ifndef DESCRIPTOR_HEAP_H
#define DESCRIPTOR_HEAP_H

#include <d3d12.h>
#include <wrl.h>
#include <variant>

namespace modelViewer::render::dx
{
	//TODO use template programming or inheritance to support other heap types as needed 
	class descriptor_heap {
	public:
		descriptor_heap(Microsoft::WRL::ComPtr<ID3D12Device>& device,
			D3D12_DESCRIPTOR_HEAP_TYPE type,
			D3D12_DESCRIPTOR_HEAP_FLAGS flags,
			UINT capacity);

		void addConstantBufferView(D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);
		void addTextureView(D3D12_SHADER_RESOURCE_VIEW_DESC& desc,
			ID3D12Resource& resource);
		[[nodiscard]] CD3DX12_GPU_DESCRIPTOR_HANDLE getHandle(UINT index) const;
		ID3D12DescriptorHeap* getHeap();
		
	private:
		
		struct view_desc
		{
			ID3D12Resource* resource;
			std::variant<D3D12_SHADER_RESOURCE_VIEW_DESC, D3D12_CONSTANT_BUFFER_VIEW_DESC> desc_variant;
		};
		
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
		UINT m_HeapCapacity;
		UINT m_DescriptorSize = 0;
		D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;
		Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
		
		std::vector<view_desc> m_Views;
		
		void resizeHeap();
	};
}


#endif //DESCRIPTOR_HEAP_H
