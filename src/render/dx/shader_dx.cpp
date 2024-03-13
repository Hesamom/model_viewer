
#include <d3dcompiler.h>
#include "shader_dx.h"

using Microsoft::WRL::ComPtr;
using namespace modelViewer::res;
using namespace modelViewer::render;


shader_dx::shader_dx(std::shared_ptr<res::shader_asset>& asset)
{
	m_Asset = asset;
}

std::string getEntryPoint(shaderType type)
{
	switch (type)
	{
		case shaderType::fragment:
			return "PS";
		case shaderType::vertex:
			return "VS";
	}
	
	return "";
}

std::string getTarget(shaderType type)
{
	switch (type)
	{
		case shaderType::fragment:
			return "ps_5_1";
		case shaderType::vertex:
			return "vs_5_1";
	}
	
	return "";
}


void shader_dx::compileToByteCode()
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	
	ComPtr<ID3DBlob> errors = nullptr;
	auto source = m_Asset->getSource();
	auto type = getTarget(m_Asset->getType());
	D3DCompile(source.c_str(), source.size(), nullptr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, getEntryPoint(m_Asset->getType()).c_str(), type.c_str(), compileFlags, 0, &m_ByteCode, &errors);
	
	if(errors != nullptr)
	{
		std::cerr << ((char*)errors->GetBufferPointer());
	}
}

Microsoft::WRL::ComPtr<ID3DBlob> shader_dx::getByteCode() const
{
	return m_ByteCode;
}

modelViewer::res::shaderType shader_dx::getType() const
{
	return m_Asset->getType();
}



