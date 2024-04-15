#include "mesh_dx.h"
#include "shader_program_dx.h"

using namespace modelViewer::render::dx;

mesh_dx::mesh_dx(std::shared_ptr<res::mesh_asset>& asset,
	Microsoft::WRL::ComPtr<ID3D12Device>& device,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & commandList)
{
	m_Device = device;
	m_CommandList = commandList;
	m_Asset = asset;
	m_PositionBuffer = std::make_unique<buffer_vertex_dx<glm::vec3>>(*asset->positions, *device.Get(), *commandList.Get());
	m_InputLayout.emplace_back("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
	m_VertexBufferViews.push_back(m_PositionBuffer->getVertexBufferView());

	if(asset->normals)
	{
		m_NormalBuffer = std::make_unique<buffer_vertex_dx<glm::vec3>>(*asset->normals, *device.Get(), *commandList.Get
		());
		m_InputLayout.emplace_back("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, m_VertexBufferViews.size(), 0,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
		m_VertexBufferViews.push_back(m_NormalBuffer->getVertexBufferView());
		
	}
	if(asset->colors)
	{
		m_ColorBuffer = std::make_unique<buffer_vertex_dx<glm::vec4>>(*asset->colors, *device.Get(), *commandList.Get());
		m_InputLayout.emplace_back("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, m_VertexBufferViews.size(), 0, 
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
		m_VertexBufferViews.push_back(m_ColorBuffer->getVertexBufferView());
	}

	if(asset->UV0)
	{
		m_UV0Buffer = std::make_unique<buffer_vertex_dx<glm::vec2>>(*asset->UV0, *device.Get(), *commandList.Get());
		m_InputLayout.emplace_back("TEXTCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, m_VertexBufferViews.size(), 0, 
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
		m_VertexBufferViews.push_back(m_UV0Buffer->getVertexBufferView());
	}
	
	m_IndexBuffer = std::make_unique<buffer_vertex_dx<unsigned int>>(*asset->indices, *device.Get(), *commandList.Get());
}

std::vector<D3D12_INPUT_ELEMENT_DESC>& mesh_dx::getLayout()
{
	return m_InputLayout;
}

unsigned int mesh_dx::getIndicesCount()
{
	return m_Asset->indices->size();
}

void mesh_dx::drawTriangles()
{
	m_CommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->DrawIndexedInstanced(getIndicesCount(),1, 0, 0, 0);
}

void mesh_dx::bind()
{
	m_CommandList->IASetVertexBuffers(0, m_VertexBufferViews.size(), m_VertexBufferViews.data());

	auto indexBufferView = m_IndexBuffer->getIndexBufferView();
	m_CommandList->IASetIndexBuffer(&indexBufferView);
}

void mesh_dx::drawLines()
{
	//TODO gotta confirm this 
	m_CommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	m_CommandList->DrawIndexedInstanced(getIndicesCount(),1, 0, 0, 0);
}

std::shared_ptr<modelViewer::res::mesh_asset> mesh_dx::getAsset()
{
	return m_Asset;
}

void mesh_dx::bindLayout(const std::shared_ptr<shader_program>& program)
{
	auto programDX = std::dynamic_pointer_cast<dx::shader_program_dx>(program);
	assert(programDX != nullptr);
	programDX->createPipelineState(getLayout());
}
