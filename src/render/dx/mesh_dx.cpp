
#include "mesh_dx.h"

modelViewer::render::mesh_dx::mesh_dx(std::shared_ptr<modelViewer::res::mesh_asset>& asset,
	Microsoft::WRL::ComPtr<ID3D12Device>& device,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & commandList)
{
	m_Device = device;
	m_CommandList = commandList;
	m_Asset = asset;
	m_PositionBuffer = std::make_unique<buffer_vertex_dx<glm::vec3>>(*asset->positions, *device.Get(), *commandList.Get());
	m_InputLayout.emplace_back("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
	
	if(asset->colors)
	{
		m_ColorBuffer = std::make_unique<buffer_vertex_dx<glm::vec4>>(*asset->colors, *device.Get(), *commandList.Get());
		m_InputLayout.emplace_back("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
	}

	m_IndexBuffer = std::make_unique<buffer_vertex_dx<unsigned int>>(*asset->indices, *device.Get(), *commandList.Get());
}

std::vector<D3D12_INPUT_ELEMENT_DESC>& modelViewer::render::mesh_dx::getLayout()
{
	return m_InputLayout;
}

unsigned int modelViewer::render::mesh_dx::getIndicesCount()
{
	return m_Asset->indices->size();
}

D3D12_VERTEX_BUFFER_VIEW modelViewer::render::mesh_dx::getVertexBufferView()
{
	return m_PositionBuffer->getVertexBufferView();
}

D3D12_INDEX_BUFFER_VIEW modelViewer::render::mesh_dx::getIndexBufferView()
{
	return m_IndexBuffer->getIndexBufferView();
}

void modelViewer::render::mesh_dx::draw()
{
	auto vertexBufferView = getVertexBufferView();
	m_CommandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	
	auto indexBufferView = getIndexBufferView();
	m_CommandList->IASetIndexBuffer(&indexBufferView);
	m_CommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->DrawIndexedInstanced(getIndicesCount(),1, 0, 0, 0);
}
