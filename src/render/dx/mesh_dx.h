
#ifndef MESH_DX_H
#define MESH_DX_H
#include "buffer_vertex_dx.h"
#include "../../resource/mesh_asset.h"
#include "../mesh.h"

namespace modelViewer::render
{
	class mesh_dx : public mesh {
	public:
		explicit mesh_dx(std::shared_ptr<modelViewer::res::mesh_asset>& asset, Microsoft::WRL::ComPtr<ID3D12Device>& device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & commandList);
		std::vector<D3D12_INPUT_ELEMENT_DESC>& getLayout();
		unsigned int getIndicesCount();

		void bindLayout(const std::shared_ptr<shader_program>& program) override;
		void bind() override;
		void drawTriangles() override;
		void drawLines() override;
		std::shared_ptr<res::mesh_asset> getAsset() override;
		
	private:
		std::unique_ptr<buffer_vertex_dx<glm::vec3>> m_PositionBuffer;
		std::unique_ptr<buffer_vertex_dx<glm::vec3>> m_NormalBuffer;
		std::unique_ptr<buffer_vertex_dx<glm::vec4>> m_ColorBuffer;
		std::unique_ptr<buffer_vertex_dx<glm::vec2>> m_UV0Buffer;
		std::vector<D3D12_VERTEX_BUFFER_VIEW> m_VertexBufferViews;
		
		std::unique_ptr<buffer_vertex_dx<unsigned int>> m_IndexBuffer;

		std::shared_ptr<res::mesh_asset> m_Asset;
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
	};
}



#endif //MESH_DX_H
