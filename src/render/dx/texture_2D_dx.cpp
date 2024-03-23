#include "texture_2D_dx.h"
#include "../../resource/DDSTextureLoader.h"
#include "../../common/string_util.h"

using namespace modelViewer::render::dx;

texture_2D_dx::texture_2D_dx(modelViewer::render::texture_setup& texture_setup, Microsoft::WRL::ComPtr<ID3D12Device>& device,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	auto fileName = ConvertAnsiToWide(texture_setup.assets[0]->getName());
	DirectX::CreateDDSTextureFromFile12( device.Get(), commandList.Get(), fileName.c_str(),m_Resource,m_UploadHeap);
}

texture_2D_dx::~texture_2D_dx()
{

}

D3D12_SHADER_RESOURCE_VIEW_DESC texture_2D_dx::getView() const
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = m_Resource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = m_Resource->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	
	return srvDesc;
}

ID3D12Resource* texture_2D_dx::getResource() const
{
	return m_Resource.Get();
}
