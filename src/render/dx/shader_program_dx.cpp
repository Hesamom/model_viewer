#include "shader_program_dx.h"
#include <d3dcompiler.h>

using namespace modelViewer::render;
using namespace modelViewer::render::dx;
using namespace Microsoft::WRL;

shader_program_dx::shader_program_dx(std::vector<std::shared_ptr<shader_dx>>& shaders,
	ID3D12Device& device,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	//TODO we need to detect the duplicate constant buffers in multiple shaders and avoid creating duplicate buffers/views 
	
	m_Shaders = shaders;
	//std::sort(m_Shaders.begin(), m_Shaders.end(), [](std::shared_ptr<shader_dx> shader1, std::shared_ptr<shader_dx> shader2) {return shader1.get()->getType() < shader2.get()->getType();});
	
	m_CommandList = commandList;
	
	for (auto& shader : m_Shaders) {
		reflectConstants(shader->getByteCode());
	}

	//std::sort(m_Constants.begin(), m_Constants.end(), [](constant_block& b1, constant_block& b2) { return b1.bindPoint < b2.bindPoint; });
	
	m_DescriptorSize = device.GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	createConstantBuffers(device);
	createRootSignature(device);
}


void shader_program_dx::createConstantBuffers(ID3D12Device& device)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = m_Constants.size();
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	device.CreateDescriptorHeap(&cbvHeapDesc,IID_PPV_ARGS(&m_Heap));
	
	UINT index = 0;
	for (auto& block : m_Constants)
	{
		std::shared_ptr<buffer_constant_generic_dx> buffer = 
			std::make_shared<buffer_constant_generic_dx>(device, (UINT)block.size, block.name.data());
		buffer->createView(device, m_Heap, index, m_DescriptorSize);
		
		m_ConstantBuffers.push_back(buffer);
		index++;
	}
}


//TODO this is based on the assumption that two variables with the same name but different types can not exist in a block, verify that
void shader_program_dx::getVariableOffset(const std::string& name, int& bufferIndex, UINT& offset)
{
	int i = 0;
	for (auto& block : m_Constants)
	{
		for (auto& variable : block.variables)
		{
			if (variable.name == name)
			{
				bufferIndex = i;
				offset = variable.offset;
			}
		}

		i++;
	}
}


void shader_program_dx::setUniform(const std::string& name, float value)
{
	setUniform(name, &value, sizeof(float));
}

void shader_program_dx::setUniform(const std::string& name, glm::mat4 value)
{
	setUniform(name, &value, sizeof(glm::mat4));
}

void shader_program_dx::setUniform(const std::string& name, glm::vec4 value)
{
	setUniform(name, &value, sizeof(glm::vec4));
}

void shader_program_dx::setUniform(const std::string& name, void* dataPtr, UINT size)
{
	int blockIndex = -1;
	UINT variableOffset = 0;

	getVariableOffset(name, blockIndex, variableOffset);
	if (variableOffset == -1)
	{
		throw std::runtime_error("uniform not found");
	}

	auto buffer = m_ConstantBuffers[blockIndex];
	buffer->set(variableOffset, dataPtr, size);
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

void shader_program_dx::createPipelineState(ID3D12Device& device, std::vector<D3D12_INPUT_ELEMENT_DESC>& layout)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	
	psoDesc.InputLayout = { layout.data(), (UINT)layout.size() };
	psoDesc.pRootSignature = m_RootSignature.Get();
	
	auto vertexShader = getVertexShader();
	auto fragmentShader = getFragShader();
	psoDesc.VS =
		{
			reinterpret_cast<BYTE*>(vertexShader->getByteCode()->GetBufferPointer()),
			vertexShader->getByteCode()->GetBufferSize()
		};
	psoDesc.PS =
		{
			reinterpret_cast<BYTE*>(fragmentShader->getByteCode()->GetBufferPointer()),
			fragmentShader->getByteCode()->GetBufferSize()
		};

	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = m_BackBufferFormat;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.DSVFormat = mDepthStencilFormat;
	device.CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSO));
}

void shader_program_dx::bind()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_Heap.Get() };
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	m_CommandList->SetPipelineState(m_PSO.Get());
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	
	for (int i = 0; i < m_Constants.size(); ++i) {

		auto handle = CD3DX12_GPU_DESCRIPTOR_HANDLE( m_Heap->GetGPUDescriptorHandleForHeapStart()); 
		handle.Offset(i, m_DescriptorSize);
		
		m_CommandList->SetGraphicsRootDescriptorTable(i, handle);
	}
	
}

void shader_program_dx::createRootSignature(ID3D12Device& device)
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


	device.CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature));
}
