
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
			UINT capacity, const std::string& name);

		int pushBack(D3D12_CONSTANT_BUFFER_VIEW_DESC& bufferDesc);
		int pushBack(D3D12_SHADER_RESOURCE_VIEW_DESC& desc, ID3D12Resource& resource);
		int pushBack(D3D12_DEPTH_STENCIL_VIEW_DESC& desc, ID3D12Resource& resource);
		
		int insert(D3D12_DEPTH_STENCIL_VIEW_DESC& desc,ID3D12Resource& resource, UINT slot = 0);
		int insert(D3D12_SHADER_RESOURCE_VIEW_DESC& desc,ID3D12Resource& resource, UINT slot = 0);
		int insert(D3D12_CONSTANT_BUFFER_VIEW_DESC& desc, UINT slot = 0);

		
		[[nodiscard]] CD3DX12_GPU_DESCRIPTOR_HANDLE getGPUHandle(UINT index) const;
		CD3DX12_CPU_DESCRIPTOR_HANDLE getCPUHandle(UINT index) const;
		
		ID3D12DescriptorHeap* getHeap();
		
		int getSize() const;
		int getCapacity() const;
		void clear();
		void copyTo(UINT start, UINT count, UINT destinationStart, descriptor_heap& destination);
		
	private:
		
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
		UINT m_Capacity = 0;
		int m_Size = 0;
		UINT m_DescriptorSize = 0;
		D3D12_DESCRIPTOR_HEAP_DESC m_Desc;
		Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
		CD3DX12_GPU_DESCRIPTOR_HANDLE m_GpuHandle;
		
		void resizeHeap();


		std::string m_Name;

		void checkSlot(UINT slot) const;

		void checkSize(UINT slot);
	};
}


#endif //DESCRIPTOR_HEAP_H
