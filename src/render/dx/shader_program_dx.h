
#ifndef SHADER_PROGRAM_DX_H
#define SHADER_PROGRAM_DX_H

#include "shader_dx.h"
#include "shader_constant.h"
#include "../shader_program.h"

namespace modelViewer::render::dx
{
	class shader_program_dx : public shader_program {
	public:
		explicit shader_program_dx(std::vector<std::shared_ptr<shader_dx>>& shaders,
			Microsoft::WRL::ComPtr<ID3D12Device>& device,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);
	
		void bind() override;
		
		void setCullFaceMode(res::cull_face_mode mode) override;
		
		bool hasUniform(const std::string &name) const override;
		void setUniform(const std::string& name, float value, bool optional) override;
		void setUniform(const std::string& name, int value, bool optional) override;
		void setUniform(const std::string& name, bool value, bool optional) override;
		void setUniform(const std::string& name, glm::mat4& value, bool optional) override;
		void setUniform(const std::string& name, glm::vec4& value, bool optional) override;
		void setUniform(const std::string& name, glm::vec3& value, bool optional) override;
		
		std::vector<shader_uniform_info> getActiveUniforms() override;
		int getActiveUniformsCount() override;

		void createPipelineState(std::vector<D3D12_INPUT_ELEMENT_DESC>& layout);
		
	private:
		void createRootSignature();
		void reflectConstants(Microsoft::WRL::ComPtr<ID3DBlob> byteCode);
		void createConstantBuffers();
		void getVariableOffset(const std::string& name, int& bufferIndex, constant_variable& offset) const;
		void setUniform(const std::string& name, void* dataPtr, UINT size, bool optional = true);
		bool isDuplicatedConstant(UINT bindPoint);
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
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_PsoDescription;
		D3D12_RASTERIZER_DESC m_RasterizerState;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
		UINT m_DescriptorSize = 0;
	};
}



#endif //SHADER_PROGRAM_DX_H
