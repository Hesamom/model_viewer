
#ifndef BUFFER_CONSTANT_H
#define BUFFER_CONSTANT_H

#include <d3d12.h>
#include <wrl.h>
#include "d3dx12.h"
#include "dx_util.h"
#include "../../common/string_util.h"

namespace modelViewer::render::dx
{
	class buffer_constant_generic_dx
	{
	public:
		buffer_constant_generic_dx(ID3D12Device& device, UINT size, const char * name)
		{
			m_Size = calcBufferByteSize(size);
			const auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			const auto buffer = CD3DX12_RESOURCE_DESC::Buffer(m_Size);
			attempt(device.CreateCommittedResource(&heapProp,
				D3D12_HEAP_FLAG_NONE,&buffer,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&mUploadBuffer)));

			auto wName = convertAnsiToWide(name);
			attempt(mUploadBuffer->SetName(wName.data()));
			attempt(mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)));
		}

		D3D12_CONSTANT_BUFFER_VIEW_DESC getView()
		{
			D3D12_GPU_VIRTUAL_ADDRESS address = mUploadBuffer.Get()->GetGPUVirtualAddress();
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = address;
			cbvDesc.SizeInBytes = m_Size;
			return cbvDesc;
		}

		ID3D12Resource* Resource()const
		{
			return mUploadBuffer.Get();
		}

		~buffer_constant_generic_dx()
		{
			if(mUploadBuffer != nullptr)
			{
				mUploadBuffer->Unmap(0, nullptr);
			}

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
