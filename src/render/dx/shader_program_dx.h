
#ifndef SHADER_PROGRAM_DX_H
#define SHADER_PROGRAM_DX_H

#include "shader_dx.h"
#include "shader_constant.h"

namespace modelViewer::render::dx
{
	class shader_program_dx {
	public:
		explicit shader_program_dx(std::vector<std::shared_ptr<shader_dx>>& shaders,
			ID3D12Device& device,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);
		void createPipelineState(ID3D12Device& device, std::vector<D3D12_INPUT_ELEMENT_DESC>& layout);
	
		void bind();
		void setUniform(const std::string& name, float value);
		void setUniform(const std::string& name, glm::mat4 value);
		void setUniform(const std::string& name, glm::vec4 value);
		
	private:
		void createRootSignature(ID3D12Device& device);
		void reflectConstants(Microsoft::WRL::ComPtr<ID3DBlob> byteCode);
		void createConstantBuffers(ID3D12Device& device);
		void getVariableOffset(const std::string& name, int& bufferIndex, UINT& offset);
		void setUniform(const std::string& name, void* dataPtr, UINT size);
	
		std::shared_ptr<shader_dx> getShaderByType(res::shaderType type);
		std::shared_ptr<shader_dx> getVertexShader();
		std::shared_ptr<shader_dx> getFragShader();
		
		const DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		const DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		
		std::vector<std::shared_ptr<shader_dx>> m_Shaders;
		std::vector<constant_block> m_Constants;
		std::vector<std::shared_ptr<buffer_constant_generic_dx>> m_ConstantBuffers;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSO;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
		UINT m_DescriptorSize = 0;

		bool isDuplicatedConstant(UINT bindPoint);
	};
}



#endif //SHADER_PROGRAM_DX_H
