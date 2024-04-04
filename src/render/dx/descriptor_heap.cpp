#define NOMINMAX

#include <d3dx12.h>
#include "descriptor_heap.h"
#include "../../common/string_util.h"

const UINT MinCapacity = 4;

modelViewer::render::dx::descriptor_heap::descriptor_heap(Microsoft::WRL::ComPtr<ID3D12Device>& device,
	D3D12_DESCRIPTOR_HEAP_TYPE type,
	D3D12_DESCRIPTOR_HEAP_FLAGS flags,
	UINT capacity, const std::string& name)
{
	assert(type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_Device = device;
	m_Capacity = std::max(capacity, MinCapacity);
	m_Desc.NumDescriptors = m_Capacity;
	m_Desc.Type = type;
	m_Desc.Flags = flags;
	m_Desc.NodeMask = 0;
	m_Name = name;
	
	m_Device->CreateDescriptorHeap(&m_Desc,IID_PPV_ARGS(&m_Heap));
	auto wideName = ConvertAnsiToWide(m_Name);
	m_Heap->SetName(wideName.data());
	m_CpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_Heap->GetCPUDescriptorHandleForHeapStart());

	if (flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
	{
		m_GpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_Heap->GetGPUDescriptorHandleForHeapStart());
	}
	
	//TODO store the size for all 4 differnt types as static values to avoid duplication and unnecessary call
	m_DescriptorSize = m_Device->GetDescriptorHandleIncrementSize( type);
}

int modelViewer::render::dx::descriptor_heap::pushBack(D3D12_CONSTANT_BUFFER_VIEW_DESC& bufferDesc)
{
	auto handle = m_CpuHandle;
	auto slot = m_Size;
	handle.Offset(slot, m_DescriptorSize);

	m_Device->CreateConstantBufferView(&bufferDesc, handle);
	m_Size++;
	if(m_Size >= m_Capacity)
	{
		resizeHeap();
	}
	return slot;
}


int modelViewer::render::dx::descriptor_heap::pushBack(D3D12_SHADER_RESOURCE_VIEW_DESC& desc,
	ID3D12Resource& resource)
{
	return insertTextureView(desc, resource, m_Size);
}

void modelViewer::render::dx::descriptor_heap::resizeHeap()
{
	m_Capacity *= 2;
	
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> newHeap;
	m_Desc.NumDescriptors = m_Capacity;
	m_Device->CreateDescriptorHeap(&m_Desc,IID_PPV_ARGS(&newHeap));
	auto newCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(newHeap->GetCPUDescriptorHandleForHeapStart());
	m_Device->CopyDescriptorsSimple(m_Size, newCPUHandle, m_CpuHandle, m_Desc.Type);
	m_Heap->Release();
	m_Heap = newHeap;
	auto wideName = ConvertAnsiToWide(m_Name);
	m_Heap->SetName(wideName.data());
	
	m_CpuHandle = newCPUHandle;
	if (m_Desc.Flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
	{
		m_GpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_Heap->GetGPUDescriptorHandleForHeapStart());
	}
}

CD3DX12_GPU_DESCRIPTOR_HANDLE modelViewer::render::dx::descriptor_heap::getGPUHandle(UINT index) const
{
	assert(m_Desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	if (index > m_Size)
	{
		throw std::invalid_argument("index out of range");
	}
	
	
	auto handle = m_GpuHandle;
	handle.Offset((int)index, m_DescriptorSize);
	return handle;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE modelViewer::render::dx::descriptor_heap::getCPUHandle(UINT index) const
{
	if (index > m_Size)
	{
		throw std::invalid_argument("index out of range");
	}
	
	auto handle = m_CpuHandle;
	handle.Offset((int)index, m_DescriptorSize);
	return handle;
}

ID3D12DescriptorHeap* modelViewer::render::dx::descriptor_heap::getHeap()
{
	return m_Heap.Get();
}

void modelViewer::render::dx::descriptor_heap::clear()
{
	m_Size = 0;
}

void modelViewer::render::dx::descriptor_heap::copyTo(UINT start,
	UINT count,
	UINT destinationStart,
	descriptor_heap& destination)
{
	if(destination.m_Desc.Type != m_Desc.Type)
	{
		throw std::invalid_argument("destination descriptor type mismatch");
	}
	
	if(start >= m_Size)
	{
		throw std::invalid_argument("start index out of range");
	}
	
	if(start + count > m_Size)
	{
		throw std::invalid_argument("end index out of range");
	}
	
	if(destinationStart > destination.m_Size)
	{
		throw std::invalid_argument("destination start index out of range");
	}

	if(destinationStart + count > destination.m_Capacity)
	{
		throw std::invalid_argument("destination capacity is not enough");
	}
	
	m_Device->CopyDescriptorsSimple(count, destination.getCPUHandle(destinationStart), getCPUHandle(start), m_Desc.Type);
	
	
	auto lastElement = destinationStart + count;
	destination.m_Size = std::max((int)lastElement, destination.m_Size);
}

int modelViewer::render::dx::descriptor_heap::insertTextureView(D3D12_SHADER_RESOURCE_VIEW_DESC& desc,
	ID3D12Resource& resource,
	UINT slot)
{
	if(slot > m_Size)
	{
		throw std::invalid_argument("slot index out of range");
	}
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_Heap->GetCPUDescriptorHandleForHeapStart());
	handle.Offset(slot, m_DescriptorSize);
	m_Device->CreateShaderResourceView(&resource, &desc, handle);
	
	if(slot == m_Size)
	{
		m_Size++;
	}
	
	if(m_Size >= m_Capacity)
	{
		resizeHeap();
	}
	return slot;
}

int modelViewer::render::dx::descriptor_heap::getSize() const
{
	return m_Size;
}

int modelViewer::render::dx::descriptor_heap::getCapacity() const
{
	return m_Capacity;
}
