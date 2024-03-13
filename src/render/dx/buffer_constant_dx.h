
#ifndef BUFFER_CONSTANT_H
#define BUFFER_CONSTANT_H

#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include "d3dx12.h"

namespace modelViewer::render
{
	template<typename T>
	class buffer_constant_dx
	{
	public:
		buffer_constant_dx(ID3D12Device& device, UINT elementsCount)
		{
			assert(elementsCount > 0);
			m_MaxElements = elementsCount;
			m_Size = calcBufferByteSize(sizeof(T));

			device.CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(m_Size * elementsCount),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&mUploadBuffer));

			mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData));
		}

		ID3D12Resource* Resource()const
		{
			return mUploadBuffer.Get();
		}

		~buffer_constant_dx()
		{
			if(mUploadBuffer != nullptr)
				mUploadBuffer->Unmap(0, nullptr);

			mMappedData = nullptr;
		}
		
		UINT getSize()
		{
			return m_Size;
		}

		void set(UINT index, const T& data)
		{
			if (index >= m_MaxElements)
			{
				throw std::range_error("index out of range");
			}
			
			memcpy(&mMappedData[index * m_Size], &data, sizeof(T));
		}

		
	private:
		static UINT calcBufferByteSize(UINT byteSize)
		{
			// Constant buffers must be a multiple of the minimum hardware
			// allocation size (usually 256 bytes).  So round up to nearest
			// multiple of 256.  We do this by adding 255 and then masking off
			// the lower 2 bytes which store all bits < 256.
			// Example: Suppose byteSize = 300.
			// (300 + 255) & ~255
			// 555 & ~255
			// 0x022B & ~0x00ff
			// 0x022B & 0xff00
			// 0x0200
			// 512
			return (byteSize + 255) & ~255;
		}

		unsigned int m_Size = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
		BYTE* mMappedData = nullptr;
		UINT m_MaxElements = 0;
	};
}
#endif //BUFFER_CONSTANT_H
