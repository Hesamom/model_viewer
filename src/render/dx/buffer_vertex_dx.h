
#ifndef BUFFER_VERTEX_DX_H
#define BUFFER_VERTEX_DX_H

#include <wrl.h>
#include <d3dcompiler.h>
#include <d3d12.h>
#include <d3dx12.h>
#include "dx_util.h"

namespace modelViewer::render::dx
{
	template<typename T> 
	class buffer_vertex_dx {
	public:
		explicit buffer_vertex_dx(std::vector<T> data, ID3D12Device& device, ID3D12GraphicsCommandList& commandList)
		{
			m_ElementSize = sizeof(T);
			m_TotalSize = m_ElementSize * data.size();
			
			attempt(device.CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(m_TotalSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

			
			attempt(device.CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(m_TotalSize),
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

			
			D3D12_SUBRESOURCE_DATA subResourceData = {};
			subResourceData.pData = data.data();
			subResourceData.RowPitch = m_TotalSize;
			subResourceData.SlicePitch = subResourceData.RowPitch;

			// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
			// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
			// the intermediate upload heap data will be copied to mBuffer.
			//commandList.ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
				//D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

			commandList.ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
				D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
			
			UpdateSubresources<1>(&commandList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

			commandList.ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
			
			attempt(D3DCreateBlob(m_TotalSize, &VertexBufferCPU));
			CopyMemory(VertexBufferCPU->GetBufferPointer(), data.data(), m_TotalSize);
		}

		D3D12_VERTEX_BUFFER_VIEW getVertexBufferView() const
		{
			D3D12_VERTEX_BUFFER_VIEW vbv;
			vbv.BufferLocation = defaultBuffer->GetGPUVirtualAddress();
			vbv.StrideInBytes = m_ElementSize;
			vbv.SizeInBytes = m_TotalSize;

			return vbv;
		}

		D3D12_INDEX_BUFFER_VIEW getIndexBufferView() const
		{
			D3D12_INDEX_BUFFER_VIEW ibv;
			ibv.BufferLocation = defaultBuffer->GetGPUVirtualAddress();
			ibv.Format = DXGI_FORMAT_R32_UINT;
			ibv.SizeInBytes = m_TotalSize;

			return ibv;
		}
		
	private:
		Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;
		size_t m_ElementSize;
		size_t m_TotalSize;
	};
}



#endif //BUFFER_VERTEX_DX_H
