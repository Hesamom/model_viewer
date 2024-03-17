#include "shader_program_dx.h"
#include <d3dcompiler.h>

using namespace modelViewer::render;
using namespace modelViewer::render::dx;
using namespace Microsoft::WRL;

shader_program_dx::shader_program_dx(std::vector<std::shared_ptr<shader_dx>>& shaders,
	Microsoft::WRL::ComPtr<ID3D12Device>& device,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	//TODO we need to detect the duplicate constant buffers in multiple shaders and avoid creating duplicate buffers/views 
	
	m_Shaders = shaders;
	m_Device = device;
	//std::sort(m_Shaders.begin(), m_Shaders.end(), [](std::shared_ptr<shader_dx> shader1, std::shared_ptr<shader_dx> shader2) {return shader1.get()->getType() < shader2.get()->getType();});
	
	m_CommandList = commandList;
	
	for (auto& shader : m_Shaders) {
		reflectConstants(shader->getByteCode());
	}

	//std::sort(m_Constants.begin(), m_Constants.end(), [](constant_block& b1, constant_block& b2) { return b1.bindPoint < b2.bindPoint; });
	
	m_DescriptorSize = m_Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	createConstantBuffers();
	createRootSignature();
	
	m_RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
}


void shader_program_dx::createConstantBuffers()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = m_Constants.size();
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	m_Device->CreateDescriptorHeap(&cbvHeapDesc,IID_PPV_ARGS(&m_Heap));
	
	UINT index = 0;
	for (auto& block : m_Constants)
	{
		std::shared_ptr<buffer_constant_generic_dx> buffer = 
			std::make_shared<buffer_constant_generic_dx>(*m_Device.Get(), (UINT)block.size, block.name.data());
		buffer->createView(*m_Device.Get(), m_Heap, index, m_DescriptorSize);
		
		m_ConstantBuffers.push_back(buffer);
		index++;
	}
}


//TODO this is based on the assumption that two variables with the same name but different types can not exist in a block, verify that
void shader_program_dx::getVariableOffset(const std::string& name, int& bufferIndex, constant_variable& var) const
{
	int i = 0;
	for (auto& block : m_Constants)
	{
		for (auto& variable : block.variables)
		{
			if (variable.name == name)
			{
				bufferIndex = i;
				var = variable;
				return;
			}
		}

		i++;
	}
}


void shader_program_dx::setUniform(const std::string& name, bool value, bool optional)
{
	setUniform(name, &value, sizeof(bool), optional);
}

void shader_program_dx::setUniform(const std::string& name, float value, bool optional)
{
	setUniform(name, &value, sizeof(float), optional);
}

void shader_program_dx::setUniform(const std::string& name, int value, bool optional)
{
	setUniform(name, &value, sizeof(int), optional);
}

void shader_program_dx::setUniform(const std::string& name, glm::mat4& value, bool optional)
{
	auto transpose = glm::transpose(value);
	setUniform(name, &transpose, sizeof(glm::mat4), optional);
}

void shader_program_dx::setUniform(const std::string& name, glm::vec3& value, bool optional)
{
	setUniform(name, &value, sizeof(glm::vec3), optional);
}


void shader_program_dx::setUniform(const std::string& name, glm::vec4& value, bool optional)
{
	setUniform(name, &value, sizeof(glm::vec4), optional);
}

void shader_program_dx::setUniform(const std::string& name, void* dataPtr, UINT size, bool optional)
{
	int blockIndex = -1;
	constant_variable variable;

	getVariableOffset(name, blockIndex, variable);
	if (variable.size == -1)
	{
		if (optional)
		{
			return;
		}
		
		throw std::runtime_error("uniform not found");
	}
	if (variable.size != size)
	{
		throw std::runtime_error("type mismatch found");
	}

	auto buffer = m_ConstantBuffers[blockIndex];
	buffer->set(variable.offset, dataPtr, size);
}

bool shader_program_dx::isDuplicatedConstant(UINT bindPoint)
{
	for (auto& b : m_Constants)
	{
		if(b.bindPoint == bindPoint)
		{
			return true;
		}
	}
	
	return false;
}

void shader_program_dx::reflectConstants(Microsoft::WRL::ComPtr<ID3DBlob> byteCode)
{
	// Create shader reflection interface
	ID3D12ShaderReflection* shaderReflection = nullptr;
	D3DReflect(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), IID_PPV_ARGS(&shaderReflection));

	// Enumerate constant buffers
	D3D12_SHADER_DESC shaderDesc;
	shaderReflection->GetDesc(&shaderDesc);

	for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i) {
		
		ID3D12ShaderReflectionConstantBuffer* constantBuffer = shaderReflection->GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC bufferDesc;
		constantBuffer->GetDesc(&bufferDesc);

		constant_block block;
		block.size = bufferDesc.Size;
		block.name = std::wstring (bufferDesc.Name, bufferDesc.Name + strlen(bufferDesc.Name));

		D3D12_SHADER_INPUT_BIND_DESC bindDesc;
		shaderReflection->GetResourceBindingDescByName(bufferDesc.Name, &bindDesc);

		block.bindPoint = bindDesc.BindPoint;

		bool duplicate = isDuplicatedConstant(block.bindPoint);
		
		std::cout << "Constant Buffer" << i << ": " << bufferDesc.Name << " #b" << bindDesc.BindPoint << std::endl;
		std::cout << "  Size: " << bufferDesc.Size << " bytes" << std::endl;
		std::cout << "  Duplicate: " << duplicate << std::endl;

		if(duplicate)
		{
			continue;
		}

		// Enumerate variables within the constant buffer
		for (UINT j = 0; j < bufferDesc.Variables; ++j) {
			ID3D12ShaderReflectionVariable* variable = constantBuffer->GetVariableByIndex(j);
			D3D12_SHADER_VARIABLE_DESC variableDesc;
			variable->GetDesc(&variableDesc);

			constant_variable variableBlock;
			variableBlock.name = variableDesc.Name;
			variableBlock.size = variableDesc.Size;
			variableBlock.offset = variableDesc.StartOffset;
			block.variables.push_back(variableBlock);

			std::cout << "  Variable #" << j << ": " << variableDesc.Name << std::endl;
			std::cout << "    Size: " << variableDesc.Size << " bytes" << std::endl;
			std::cout << "    Offset: " << variableDesc.StartOffset << " bytes" << std::endl;
		}
		m_Constants.push_back(block);
	}
}

std::shared_ptr<shader_dx> shader_program_dx::getVertexShader()
{
	return getShaderByType(modelViewer::res::shaderType::vertex);
}


std::shared_ptr<shader_dx> shader_program_dx::getFragShader()
{
	return getShaderByType(modelViewer::res::shaderType::fragment);
}

std::shared_ptr<shader_dx> shader_program_dx::getShaderByType(modelViewer::res::shaderType type)
{
	for (auto& shader : m_Shaders)
	{
		if(shader->getType() == type)
		{
			return shader;
		}
	}

	return nullptr;
}

void shader_program_dx::createPipelineState(std::vector<D3D12_INPUT_ELEMENT_DESC>& layout)
{
	ZeroMemory(&m_PsoDescription, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	m_PsoDescription.InputLayout = { layout.data(), (UINT)layout.size() };
	m_PsoDescription.pRootSignature = m_RootSignature.Get();
	
	auto vertexShader = getVertexShader();
	auto fragmentShader = getFragShader();
	m_PsoDescription.VS =
		{
			reinterpret_cast<BYTE*>(vertexShader->getByteCode()->GetBufferPointer()),
			vertexShader->getByteCode()->GetBufferSize()
		};
	m_PsoDescription.PS =
		{
			reinterpret_cast<BYTE*>(fragmentShader->getByteCode()->GetBufferPointer()),
			fragmentShader->getByteCode()->GetBufferSize()
		};

	m_PsoDescription.RasterizerState = m_RasterizerState;
	m_PsoDescription.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	m_PsoDescription.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	m_PsoDescription.SampleMask = UINT_MAX;
	m_PsoDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_PsoDescription.NumRenderTargets = 1;
	m_PsoDescription.RTVFormats[0] = m_BackBufferFormat;
	m_PsoDescription.SampleDesc.Count = 1;
	m_PsoDescription.SampleDesc.Quality = 0;
	m_PsoDescription.DSVFormat = mDepthStencilFormat;
	m_Device->CreateGraphicsPipelineState(&m_PsoDescription, IID_PPV_ARGS(&m_PSO));
}

void shader_program_dx::bind()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_Heap.Get() };
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	m_CommandList->SetPipelineState(m_PSO.Get());
	//TODO set rasterizer state 
	//m_CommandList->RSSetState(m_RasterizerState.Get());
	
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	
	for (int i = 0; i < m_Constants.size(); ++i) {

		auto handle = CD3DX12_GPU_DESCRIPTOR_HANDLE( m_Heap->GetGPUDescriptorHandleForHeapStart()); 
		handle.Offset(i, m_DescriptorSize);
		
		m_CommandList->SetGraphicsRootDescriptorTable(i, handle);
	}
	
}

void shader_program_dx::createRootSignature()
{
	UINT baseRegisterIndex = 0;
	std::vector<CD3DX12_ROOT_PARAMETER> parameters;
	
	for (auto& block : m_Constants)
	{
		auto cbvTable = new CD3DX12_DESCRIPTOR_RANGE();
		cbvTable->Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, block.bindPoint);

		CD3DX12_ROOT_PARAMETER parameter;
		parameter.InitAsDescriptorTable(1, cbvTable);
		parameters.push_back(parameter);
		
		baseRegisterIndex++;
	}
	
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(parameters.size(), parameters.data(), 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if(errorBlob != nullptr)
	{
		std::cerr << ((char*)errorBlob->GetBufferPointer());
	}


	m_Device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature));
}

void shader_program_dx::setCullFaceMode(modelViewer::res::cull_face_mode mode)
{
	switch (mode)
	{
		case res::cull_face_mode::disabled:
			m_RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
			break;
		case res::cull_face_mode::front:
			m_RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
			break;
		case res::cull_face_mode::back:
			m_RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
			break;
	}
	
	//TODO create a new rasterizer state 
}

std::vector<shader_uniform_info> shader_program_dx::getActiveUniforms()
{
	return std::vector<shader_uniform_info>();
}

int shader_program_dx::getActiveUniformsCount()
{
	return 0;
}

bool shader_program_dx::hasUniform(const std::string& name) const
{
	int blockIndex = -1;
	constant_variable var;
	getVariableOffset(name, blockIndex, var);
	return blockIndex > -1 && var.offset > -1;
}


