#define NOMINMAX

#include <d3dx12.h>
#include "descriptor_heap.h"

const UINT MinCapacity = 4;

modelViewer::render::dx::descriptor_heap::descriptor_heap(Microsoft::WRL::ComPtr<ID3D12Device>& device,
	D3D12_DESCRIPTOR_HEAP_TYPE type,
	D3D12_DESCRIPTOR_HEAP_FLAGS flags,
	UINT capacity)
{
	assert(type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_Device = device;
	m_HeapCapacity = std::min(capacity, MinCapacity);
	m_HeapDesc.NumDescriptors = m_HeapCapacity;
	m_HeapDesc.Type = type;
	m_HeapDesc.Flags = flags;
	m_HeapDesc.NodeMask = 0;

	m_Device->CreateDescriptorHeap(&m_HeapDesc,IID_PPV_ARGS(&m_Heap));
	m_DescriptorSize = m_Device->GetDescriptorHandleIncrementSize( type);
}

void modelViewer::render::dx::descriptor_heap::addConstantBufferView(D3D12_CONSTANT_BUFFER_VIEW_DESC& desc)
{
	auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_Heap->GetCPUDescriptorHandleForHeapStart());
	handle.Offset((int)m_Views.size(), m_DescriptorSize);

	m_Device->CreateConstantBufferView(&desc, handle);
	view_desc desc1;
	desc1.resource = nullptr;
	desc1.desc_variant = desc;
	
	m_Views.push_back(desc1);
	if(m_Views.size() >= m_HeapCapacity)
	{
		resizeHeap();
	}
}

void modelViewer::render::dx::descriptor_heap::addTextureView(D3D12_SHADER_RESOURCE_VIEW_DESC& desc,
	ID3D12Resource& resource)
{
	/*D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = bricksTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; 
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = bricksTex->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;*/

	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_Heap->GetCPUDescriptorHandleForHeapStart());
	handle.Offset((int)m_Views.size(), m_DescriptorSize);
	m_Device->CreateShaderResourceView(&resource, &desc, handle);

	view_desc desc1;
	desc1.resource = &resource;
	desc1.desc_variant = desc;

	m_Views.push_back(desc1);
}

void modelViewer::render::dx::descriptor_heap::resizeHeap()
{
	m_HeapCapacity *= 2;

	m_Heap->Release();
	m_HeapDesc.NumDescriptors = m_HeapCapacity;
	m_Device->CreateDescriptorHeap(&m_HeapDesc,IID_PPV_ARGS(&m_Heap));
	
	int size = 0;
	for (auto& desc : m_Views) {

		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_Heap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(size, m_DescriptorSize);
		size++;

		if (std::holds_alternative<D3D12_SHADER_RESOURCE_VIEW_DESC>(desc.desc_variant)) {
			m_Device->CreateShaderResourceView(desc.resource, &std::get<D3D12_SHADER_RESOURCE_VIEW_DESC>(desc
			.desc_variant),
			    handle);
		}
		else
		{
			m_Device->CreateConstantBufferView(&std::get<D3D12_CONSTANT_BUFFER_VIEW_DESC>(desc.desc_variant), handle);
		}
	}
}

CD3DX12_GPU_DESCRIPTOR_HANDLE modelViewer::render::dx::descriptor_heap::getHandle(UINT index) const
{
	if (index >= m_Views.size())
	{
		throw std::invalid_argument("index out of range");
	}
	
	auto handle = CD3DX12_GPU_DESCRIPTOR_HANDLE( m_Heap->GetGPUDescriptorHandleForHeapStart());
	handle.Offset((int)index, m_DescriptorSize);
	return handle;
}

ID3D12DescriptorHeap* modelViewer::render::dx::descriptor_heap::getHeap()
{
	return m_Heap.Get();
}
