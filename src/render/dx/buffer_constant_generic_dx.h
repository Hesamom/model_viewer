
#ifndef BUFFER_CONSTANT_H
#define BUFFER_CONSTANT_H

#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include "d3dx12.h"

namespace modelViewer::render
{
	class buffer_constant_generic_dx
	{
	public:
		buffer_constant_generic_dx(ID3D12Device& device, UINT size, const wchar_t * name)
		{
			m_Size = calcBufferByteSize(size);
			device.CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(m_Size),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&mUploadBuffer));
			
			mUploadBuffer->SetName(name);
			
			mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData));
		}
		
		void createView(ID3D12Device& device, const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& heap, UINT viewIndex, UINT viewSize)
		{
			D3D12_GPU_VIRTUAL_ADDRESS address = mUploadBuffer.Get()->GetGPUVirtualAddress();

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = address;
			cbvDesc.SizeInBytes = m_Size;

			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(viewIndex, viewSize);

			device.CreateConstantBufferView(&cbvDesc, handle);
		}

		ID3D12Resource* Resource()const
		{
			return mUploadBuffer.Get();
		}

		~buffer_constant_generic_dx()
		{
			if(mUploadBuffer != nullptr)
				mUploadBuffer->Unmap(0, nullptr);

			mMappedData = nullptr;
		}
		
		UINT getSize()
		{
			return m_Size;
		}

		void set(UINT offset, void* data, UINT size)
		{
			if (offset + size > m_Size)
			{
				throw std::range_error("index out of range");
			}
			
			memcpy(&mMappedData[offset], data, size);
		}

		
	private:
		static UINT calcBufferByteSize(UINT byteSize)
		{
			return (byteSize + 255) & ~255;
		}

		UINT m_Size = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
		BYTE* mMappedData = nullptr;
	};
}
#endif //BUFFER_CONSTANT_H
