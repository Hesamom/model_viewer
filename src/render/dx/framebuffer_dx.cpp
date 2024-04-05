
#include <d3d12.h>
#include <d3dx12.h>
#include "framebuffer_dx.h"
#include "texture_2D_dx.h"

using namespace modelViewer::render::dx;
using namespace modelViewer::render;

void framebuffer_dx::unbind()
{
	if (m_BoundDepth && m_DepthTexture)
	{
		m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthTexture->getResource(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
		m_BoundDepth = false;
	}
}

framebuffer_dx::framebuffer_dx(Microsoft::WRL::ComPtr<ID3D12Device>& device,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList,  std::string& name)
{
	m_Device = device;
	m_commandList = commandList;
	m_Name = name;
}

void framebuffer_dx::createDepthTexture(int width,int height,bool enableDepthCompare)
{
	auto textureName = std::string (m_Name + "_depth");
	m_DepthTexture = std::make_shared<texture_2D_dx>(width, height, textureName, m_Device);
	
	ZeroMemory(&m_DepthViewport, sizeof(D3D12_VIEWPORT));
	m_DepthViewport.MaxDepth = 1.0f;
	m_DepthViewport.Width = static_cast<float>(width);
	m_DepthViewport.Height = static_cast<float>(height);

	m_DepthScissorRect.left = 0;
	m_DepthScissorRect.top = 0;
	m_DepthScissorRect.right = width;
	m_DepthScissorRect.bottom = height;

	if (!m_DepthDescriptorHeap)
	{
		m_DepthDescriptorHeap = std::make_shared<descriptor_heap>(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1, m_Name + "_depth_desc_heap");
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	m_DepthDescriptorHeap->insert(dsvDesc, *m_DepthTexture->getResource(), 0);
}

void framebuffer_dx::clearDepthBuffer()
{
	if (m_DepthTexture)
	{
		m_commandList->ClearDepthStencilView(m_DepthDescriptorHeap->getCPUHandle(0), D3D12_CLEAR_FLAG_DEPTH | 
		D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0,
			0, nullptr);
	}
}

std::shared_ptr<modelViewer::render::texture> framebuffer_dx::getDepthMap()
{
	return m_DepthTexture;
}

void framebuffer_dx::clearColorBuffer(const glm::vec4& color)
{
	//float clearColor[4] = { color.r, color.g, color.b, 1.0f };
	//m_commandList->ClearRenderTargetView(getCurrentBackBufferView(), clearColor, 0, nullptr);
}

void framebuffer_dx::attachDepthTexture()
{
	if (m_DepthTexture)
	{
		m_commandList->RSSetViewports(1, &m_DepthViewport);
		m_commandList->RSSetScissorRects(1, &m_DepthScissorRect);


		m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition( m_DepthTexture->getResource(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

		m_commandList->OMSetRenderTargets(0, nullptr, false, &m_DepthDescriptorHeap->getCPUHandle(0));
		m_BoundDepth = true;
	}
}

void framebuffer_dx::attachCubeMapFace(int index)
{
	throw std::runtime_error("Not implemented");
}

void framebuffer_dx::attachDepthTextureArray(int layer)
{
	throw std::runtime_error("Not implemented");
}

void framebuffer_dx::createArrayDepthTexture(int width,
	int height,
	int layers,
	bool enableDepthCompare)
{
	throw std::runtime_error("Not implemented");
}

void framebuffer_dx::createCubeMap(int size)
{
	throw std::runtime_error("Not implemented");
}

void framebuffer_dx::createColorBuffer(int width, int height)
{
	throw std::runtime_error("Not implemented");
}

std::shared_ptr<texture> framebuffer_dx::getDepthMapArray()
{
	throw std::runtime_error("Not implemented");
}

std::shared_ptr<texture> framebuffer_dx::getColorCubeMap()
{
	throw std::runtime_error("Not implemented");
}
