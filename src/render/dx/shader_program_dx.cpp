#include "shader_program_dx.h"
#include "dx_util.h"
#include <d3dcompiler.h>

#include "uniform_buffer_dx.h"

using namespace modelViewer::render;
using namespace modelViewer::render::dx;
using namespace Microsoft::WRL;

std::shared_ptr<descriptor_heap> shader_program_dx::m_CBV_SRV_UAV_GPUHeap;

CD3DX12_STATIC_SAMPLER_DESC shader_program_dx::staticSamplers[6]
{
    {
        0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP
    },
    {
        1,
        D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP
    },
    {
        2,
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP
    },
    {
        3,
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP
    },
    {
        4,
        D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP
    },
    {
        5,
        D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP
    },
};

void shader_program_dx::createDescriptorHeaps()
{
    m_CBVHeap = std::make_shared<descriptor_heap>(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                                  D3D12_DESCRIPTOR_HEAP_FLAG_NONE, m_Constants.size(),
                                                  "constant desc heap");
    m_TexturesHeap = std::make_shared<descriptor_heap>(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                                       D3D12_DESCRIPTOR_HEAP_FLAG_NONE, m_TextureSlots.size(),
                                                       "texture desc heap");

    if (m_CBV_SRV_UAV_GPUHeap == nullptr)
    {
        m_CBV_SRV_UAV_GPUHeap = std::make_shared<descriptor_heap>(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                                                  D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 256,
                                                                  "shared cbv_srv heap");
    }
}

shader_program_dx::shader_program_dx(std::vector<std::shared_ptr<shader_dx>>& shaders,
                                     ComPtr<ID3D12Device>& device,
                                     ComPtr<ID3D12GraphicsCommandList>& commandList)
{
    //TODO we need to detect the duplicate constant buffers in multiple shaders and avoid creating duplicate buffers/views 

    m_Shaders = shaders;
    m_Device = device;
    m_CommandList = commandList;

    //TODO we need to get input layout info from the vertex shader and make sure the mesh can satisfy the requirements when building the pipeline state 

    for (auto& shader: m_Shaders)
    {
        reflectShader(shader);
    }

    createDescriptorHeaps();
    createConstantBuffers();
    createRootSignature();
}


void shader_program_dx::createConstantBuffers()
{
    UINT index = 0;
    for (auto& block: m_Constants)
    {
        if (block.name == "u_globals")
        {
            std::shared_ptr<buffer_constant_generic_dx> buffer =
                    std::make_shared<buffer_constant_generic_dx>(*m_Device.Get(), (UINT) block.size, block.name.data());
            auto view = buffer->getView();
            m_CBVHeap->insert(view, index);
            m_ConstantBuffers.push_back(buffer);
        }

        index++;
    }
}

void shader_program_dx::getVariableOffsetWithSubscript(std::string& blockName, std::string& memberName,
                                                       int& bufferIndex, constant_variable& var) const
{
    int i = 0;
    for (auto& block: m_Constants)
    {
        if (block.name == blockName)
        {
            for (auto& variable: block.variables)
            {
                if (variable.name == memberName)
                {
                    bufferIndex = i;
                    var = variable;
                    return;
                }
            }
        }
    }
}

//TODO this is based on the assumption that two variables with the same name but different types can not exist in a block, verify that
void shader_program_dx::getVariableOffset(const std::string& name, int& bufferIndex, constant_variable& var) const
{
    /*const auto subscriptIndex = name.find(SubscriptCharacter);
    if (subscriptIndex > 0 && subscriptIndex < name.size())
    {
        auto blockName = name.substr(0, subscriptIndex);
        auto memberName = name.substr(subscriptIndex + 1);
        getVariableOffsetWithSubscript(blockName, memberName, bufferIndex, var);
        return;
    }*/

    int i = 0;
    for (auto& block: m_Constants)
    {
        for (auto& variable: block.variables)
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
    if (variable.absluteOffset == -1 || blockIndex >= m_ConstantBuffers.size())
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
    buffer->set(variable.absluteOffset, dataPtr, size);
}

bool shader_program_dx::isDuplicatedConstant(UINT bindPoint) const
{
    for (auto& b: m_Constants)
    {
        if (b.bindPoint == bindPoint)
        {
            return true;
        }
    }

    return false;
}

/*void reflectShaderVariable(ID3D12ShaderReflectionType* memberType, constant_variable* variable)
{
	D3D12_SHADER_TYPE_DESC typeDesc;
	memberType->GetDesc(&typeDesc);
	if (typeDesc.Class != D3D_SVC_STRUCT)
	{
		return;
	}
	
	for (int i = 0; i < typeDesc.Members; i++) {
		auto info = memberType->GetMemberTypeByIndex(i);
		D3D12_SHADER_TYPE_DESC memDesc;
		info->GetDesc(&memDesc);
		constant_variable var;
		var.absluteOffset = memDesc.Offset + variable->absluteOffset;
		var.name = memDesc.Name;
		var.size = 0;
		variable->children.push_back(var);
		reflectShaderVariable(info, &var);
	}
}*/

void shader_program_dx::reflectTextures(ID3D12ShaderReflection* reflection, D3D12_SHADER_DESC shaderDesc)
{
    for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
    {
        D3D12_SHADER_INPUT_BIND_DESC bindDesc;
        reflection->GetResourceBindingDesc(i, &bindDesc);

        if (bindDesc.Type == D3D_SIT_TEXTURE)
        {
            // Process sampler or texture
            // bindDesc.Name contains the name of the resource
            texture_slot slot;
            slot.name = bindDesc.Name;
            slot.slot = bindDesc.BindPoint;
            m_Textures.push_back(slot);

            shader_texture_slot textureSlot;
            textureSlot.name = bindDesc.Name;
            textureSlot.type = getTextureType(bindDesc.Dimension);
            m_TextureSlots.push_back(textureSlot);

            std::cout << "Texture" << i << ": " << slot.name << " #t" << bindDesc.BindPoint << std::endl;
        }
    }
}

void shader_program_dx::reflectConstantBuffers(ID3D12ShaderReflection* reflection, D3D12_SHADER_DESC shaderDesc)
{
    for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i)
    {
        ID3D12ShaderReflectionConstantBuffer* constantBuffer = reflection->GetConstantBufferByIndex(i);
        D3D12_SHADER_BUFFER_DESC bufferDesc;
        constantBuffer->GetDesc(&bufferDesc);


        constant_block block;
        block.size = bufferDesc.Size;
        block.name = std::string(bufferDesc.Name, bufferDesc.Name + strlen(bufferDesc.Name));

        D3D12_SHADER_INPUT_BIND_DESC bindDesc;
        reflection->GetResourceBindingDescByName(bufferDesc.Name, &bindDesc);

        block.bindPoint = bindDesc.BindPoint;

        bool duplicate = isDuplicatedConstant(block.bindPoint);

        std::cout << "Constant Buffer" << i << ": " << bufferDesc.Name << " #b" << bindDesc.BindPoint << std::endl;
        std::cout << "  Size: " << bufferDesc.Size << " bytes" << std::endl;
        std::cout << "  Duplicate: " << duplicate << std::endl;

        if (duplicate)
        {
            continue;
        }

        // Enumerate variables within the constant buffer
        for (UINT j = 0; j < bufferDesc.Variables; ++j)
        {
            ID3D12ShaderReflectionVariable* variable = constantBuffer->GetVariableByIndex(j);
            D3D12_SHADER_VARIABLE_DESC variableDesc;
            variable->GetDesc(&variableDesc);

            constant_variable variableBlock;
            variableBlock.name = variableDesc.Name;
            variableBlock.size = variableDesc.Size;
            variableBlock.absluteOffset = variableDesc.StartOffset;
            block.variables.push_back(variableBlock);

            //auto varType = variable->GetType();
            //reflectShaderVariable(varType, &variableBlock);

            std::cout << "  Variable #" << j << ": " << variableDesc.Name << std::endl;
            std::cout << "    Size: " << variableDesc.Size << " bytes" << std::endl;
            std::cout << "    Offset: " << variableDesc.StartOffset << " bytes" << std::endl;
        }
        m_Constants.push_back(block);
    }
}

void shader_program_dx::reflectShader(std::shared_ptr<shader_dx>& shader)
{
    std::cout << "Shader reflection info for shader " << shader->getTypeName() << ":" << std::endl;
    auto shaderByteCode = shader->getByteCode();
    // Create shader reflection interface
    ID3D12ShaderReflection* reflection = nullptr;
    attempt(D3DReflect(shaderByteCode->GetBufferPointer(), shaderByteCode->GetBufferSize(), IID_PPV_ARGS(&reflection)));
    D3D12_SHADER_DESC shaderDesc;
    reflection->GetDesc(&shaderDesc);
    
    reflectTextures(reflection, shaderDesc);
    reflectConstantBuffers(reflection, shaderDesc);
}

std::shared_ptr<shader_dx> shader_program_dx::getVertexShader()
{
    return getShaderByType(res::shaderType::vertex);
}


std::shared_ptr<shader_dx> shader_program_dx::getFragShader()
{
    return getShaderByType(res::shaderType::fragment);
}

std::shared_ptr<shader_dx> shader_program_dx::getShaderByType(res::shaderType type)
{
    for (auto& shader: m_Shaders)
    {
        if (shader->getType() == type)
        {
            return shader;
        }
    }

    return nullptr;
}

void shader_program_dx::createPipelineState(std::vector<D3D12_INPUT_ELEMENT_DESC>& layout)
{
    ZeroMemory(&m_PsoDescription, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

    m_PsoDescription.InputLayout = {layout.data(), (UINT) layout.size()};
    m_PsoDescription.pRootSignature = m_RootSignature.Get();

    auto vertexShader = getVertexShader();
    auto fragmentShader = getFragShader();
    m_PsoDescription.VS =
    {
        reinterpret_cast<BYTE *>(vertexShader->getByteCode()->GetBufferPointer()),
        vertexShader->getByteCode()->GetBufferSize()
    };
    m_PsoDescription.PS =
    {
        reinterpret_cast<BYTE *>(fragmentShader->getByteCode()->GetBufferPointer()),
        fragmentShader->getByteCode()->GetBufferSize()
    };

    m_PsoDescription.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    m_PsoDescription.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    m_PsoDescription.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    m_PsoDescription.SampleMask = UINT_MAX;
    m_PsoDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    m_PsoDescription.NumRenderTargets = 1;
    m_PsoDescription.RTVFormats[0] = m_BackBufferFormat;
    m_PsoDescription.SampleDesc.Count = 1;
    m_PsoDescription.SampleDesc.Quality = 0;
    m_PsoDescription.DSVFormat = mDepthStencilFormat;
    attempt(m_Device->CreateGraphicsPipelineState(&m_PsoDescription, IID_PPV_ARGS(&m_PSO)));
}

void shader_program_dx::bind()
{
    auto start = m_CBV_SRV_UAV_GPUHeap->getSize();
    m_CBVHeap->copyTo(0, m_CBVHeap->getSize(), start, *m_CBV_SRV_UAV_GPUHeap);
    m_TexturesHeap->copyTo(0, m_TexturesHeap->getSize(), start + m_CBVHeap->getSize(), *m_CBV_SRV_UAV_GPUHeap);

    m_CommandList->SetPipelineState(m_PSO.Get());
    m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

    for (int i = 0; i < m_Constants.size() + m_Textures.size(); ++i)
    {
        m_CommandList->SetGraphicsRootDescriptorTable(i, m_CBV_SRV_UAV_GPUHeap->getGPUHandle(start + i));
    }
}

void shader_program_dx::createRootSignature()
{
    std::vector<CD3DX12_ROOT_PARAMETER> parameters;

    for (auto& block: m_Constants)
    {
        auto cbvTable = new CD3DX12_DESCRIPTOR_RANGE();
        cbvTable->Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, block.bindPoint);

        CD3DX12_ROOT_PARAMETER parameter;
        parameter.InitAsDescriptorTable(1, cbvTable);
        parameters.push_back(parameter);
    }

    for (auto& texture: m_Textures)
    {
        auto texTable = new CD3DX12_DESCRIPTOR_RANGE();
        texTable->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, texture.slot);

        CD3DX12_ROOT_PARAMETER parameter;
        parameter.InitAsDescriptorTable(1, texTable);
        parameters.push_back(parameter);
    }

    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(parameters.size(), parameters.data(), 6, staticSamplers,
                                            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                                serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        std::cerr << ((char *) errorBlob->GetBufferPointer());
    }


    m_Device->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&m_RootSignature));
}

void shader_program_dx::setCullFaceMode(res::cull_face_mode mode)
{
    switch (mode)
    {
        case res::cull_face_mode::disabled:
            m_PsoDescription.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
            break;
        case res::cull_face_mode::front:
            m_PsoDescription.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
            break;
        case res::cull_face_mode::back:
            m_PsoDescription.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
            break;
    }
    updatePipeline();
}

void shader_program_dx::setAlphaBlending(bool enabled)
{
    auto blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    if (!enabled)
    {
        m_PsoDescription.BlendState = blendState;
        updatePipeline();
        return;
    }

    blendState.AlphaToCoverageEnable = false;
    blendState.IndependentBlendEnable = false;
    blendState.RenderTarget[0].BlendEnable = true;
    blendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    blendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    blendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    m_PsoDescription.BlendState = blendState;
    updatePipeline();
}


bool shader_program_dx::hasUniform(const std::string& name) const
{
    int blockIndex = -1;
    constant_variable var;
    getVariableOffset(name, blockIndex, var);
    return blockIndex > -1 && var.absluteOffset > -1;
}

void shader_program_dx::updatePipeline()
{
    auto temp = m_PsoDescription;
    attempt(m_Device->CreateGraphicsPipelineState(&temp, IID_PPV_ARGS(&m_PSO)));
}

D3D12_COMPARISON_FUNC getDepthFunction(modelViewer::res::depth_buffer_compare compare)
{
    switch (compare)
    {
        case modelViewer::res::never:
            return D3D12_COMPARISON_FUNC_NEVER;
        case modelViewer::res::less:
            return D3D12_COMPARISON_FUNC_LESS;
        case modelViewer::res::equal:
            return D3D12_COMPARISON_FUNC_EQUAL;
        case modelViewer::res::lessOrEqual:
            return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case modelViewer::res::greater:
            return D3D12_COMPARISON_FUNC_GREATER;
        case modelViewer::res::notEqual:
            return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        case modelViewer::res::greaterOrEqual:
            return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        case modelViewer::res::always:
            return D3D12_COMPARISON_FUNC_ALWAYS;
        default:
            throw std::runtime_error("not supported");
    }
}

void shader_program_dx::setDepthMap(res::depth_buffer_options& options)
{
    m_PsoDescription.DepthStencilState.DepthEnable = options.enabled;
    m_PsoDescription.DepthStencilState.DepthFunc = getDepthFunction(options.compare);
    m_PsoDescription.DepthStencilState.DepthWriteMask = options.writeEnabled
                                                            ? D3D12_DEPTH_WRITE_MASK_ALL
                                                            : D3D12_DEPTH_WRITE_MASK_ZERO;
    updatePipeline();
}

const std::vector<shader_texture_slot>& shader_program_dx::getTextureSlots()
{
    return m_TextureSlots;
}

shader_texture_type shader_program_dx::getTextureType(D3D_SRV_DIMENSION dimension)
{
    switch (dimension)
    {
        case D3D_SRV_DIMENSION_TEXTURE1D:
            return shader_texture_type::texture1D;
        case D3D_SRV_DIMENSION_TEXTURE2D:
            return shader_texture_type::texture2D;
        case D3D_SRV_DIMENSION_TEXTURE3D:
            return shader_texture_type::texture3D;
        case D3D_SRV_DIMENSION_TEXTURECUBE:
            return shader_texture_type::textureCube;
        case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
            return shader_texture_type::texture1DArray;
        case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
            return shader_texture_type::texture2DArray;
        default:
            throw std::runtime_error("Unsupported texture dimension");
    }
}

void shader_program_dx::bindTexture(int slotIndex, std::shared_ptr<texture>& texture)
{
    auto text = std::dynamic_pointer_cast<texture_dx>(texture);
    if (text == nullptr)
    {
        throw std::runtime_error("texture_dx is nullptr");
    }

    auto view = text->getView();
    auto res = text->getResource();
    if (m_TextureDesc.contains(slotIndex))
    {
        auto heapSlot = m_TexturesHeap->insert(view, *res, m_TextureDesc.at(slotIndex));
        m_TextureDesc[slotIndex] = heapSlot;
        return;
    }

    auto heapSlot = m_TexturesHeap->pushBack(view, *res);
    m_TextureDesc[slotIndex] = heapSlot;
}

void shader_program_dx::clearHeap()
{
    if (m_CBV_SRV_UAV_GPUHeap)
    {
        m_CBV_SRV_UAV_GPUHeap->clear();
    }
}

void shader_program_dx::setGPUHeap(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
    if (m_CBV_SRV_UAV_GPUHeap)
    {
        ID3D12DescriptorHeap* descriptorHeaps[] = {m_CBV_SRV_UAV_GPUHeap->getHeap()};
        commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
    }
}

int shader_program_dx::getTextureSlot(const std::string& textureName)
{
    int slotIndex = 0;
    for (auto& slot: m_TextureSlots)
    {
        if (slot.name == textureName)
        {
            return slotIndex;
        }
        slotIndex++;
    }
    return -1;
}

bool shader_program_dx::bindTexture(const std::string& name, std::shared_ptr<texture>& texture)
{
    auto slot = getTextureSlot(name);
    if (slot >= 0)
    {
        bindTexture(slot, texture);
        return true;
    }
    return false;
}

void shader_program_dx::setTopology(topology_mode topology)
{
    if (m_CurrentTopology == topology)
    {
        return;
    }

    switch (topology)
    {
        case topology_mode::triangles:
            m_PsoDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            break;
        case topology_mode::lines:
            m_PsoDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
            break;
        case topology_mode::point:
            m_PsoDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
            break;
    }

    m_CurrentTopology = topology;
    updatePipeline();
}

topology_mode shader_program_dx::getTopology()
{
    return m_CurrentTopology;
}

bool shader_program_dx::hasUniformBufferSlot(const std::string& name) const
{
    for (auto& c: m_Constants)
    {
        if (c.name == name)
        {
            return true;
        }
    }

    return false;
}

void shader_program_dx::setUniformBuffer(std::shared_ptr<uniform_buffer>& buffer, const std::string& name)
{
    if (m_UniformBufferBindings.contains(name) && m_UniformBufferBindings.at(name) == buffer)
    {
        return;
    }

    int index = 0;
    for (auto& c: m_Constants)
    {
        if (c.name == name)
        {
            assert(c.size <= buffer->getSize());
            auto bufferDx = std::dynamic_pointer_cast<uniform_buffer_dx>(buffer);
            assert(bufferDx != nullptr);
            auto view = bufferDx->getView();
            m_CBVHeap->insert(view, index);
            m_UniformBufferBindings[name] = buffer;
        }
        index++;
    }
}
