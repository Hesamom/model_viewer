#include "texture_2D_dx.h"
#include "../../resource/DDSTextureLoader.h"
#include "../../common/string_util.h"

using namespace modelViewer::render::dx;

texture_2D_dx::texture_2D_dx(modelViewer::render::texture_setup& texture_setup, Microsoft::WRL::ComPtr<ID3D12Device>& device,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	if (texture_setup.type != res::texture_asset_type::texture2D)
	{
		throw std::runtime_error("Invalid texture setup type");
	}
	
	auto firstAsset = texture_setup.assets[0];
	if (firstAsset->getName().ends_with(".dds"))
	{
		auto fileName = ConvertAnsiToWide(texture_setup.assets[0]->getName());
		DirectX::CreateDDSTextureFromFile12( device.Get(), commandList.Get(), fileName.c_str(),m_Resource,m_UploadHeap);
		return;
	}
	
	createTextureFromRawData(texture_setup, device, commandList);
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

void texture_2D_dx::createTextureFromRawData(modelViewer::render::texture_setup& setup,
	Microsoft::WRL::ComPtr<ID3D12Device>& device,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	int width = setup.assets[0]->getWidth();
	int height = setup.assets[0]->getHeight();
	auto format = getTextureFormat(setup.assets[0], false);
	// Create a texture resource
	CD3DX12_RESOURCE_DESC desc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		0,
		width,
		height,
		1,
		setup.isMipMapActive ? 0 : 1,
		format,
		1,
		0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_NONE
	);

	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&m_Resource)
	);

	// Upload the pixel data to the GPU
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = setup.assets[0]->getContent();
	auto channelCount = setup.assets[0]->getChannelCount();
	textureData.RowPitch = width * channelCount;
	textureData.SlicePitch = textureData.RowPitch * height;

	
	// Transition the texture to a copy destination state
	auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);;
	UINT64 requiredSize = GetRequiredIntermediateSize( m_Resource.Get(), 0, 1 );
	
	device->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(requiredSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_UploadHeap)
	);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	// Copy the pixel data to the upload heap
	UpdateSubresources(commandList.Get(), m_Resource.Get(), m_UploadHeap.Get(), 0, 0, 1, &textureData);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}

int texture_2D_dx::getBytesPerPixel(const DXGI_FORMAT& format)
{
	switch (format)
	{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return 4;
		case DXGI_FORMAT_R8_UNORM:
			return 1;
		case DXGI_FORMAT_R8G8_UNORM:
			return 2;
	}
	
	throw std::runtime_error("not supported format format");
}

//TODO may have to query the format support 
//https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_feature_data_format_support
DXGI_FORMAT texture_2D_dx::getTextureFormat(std::shared_ptr<modelViewer::res::texture_asset>& texture, bool compressed)
{
	int channels = texture->getChannelCount();
	switch (channels) {
		case 1:
			if (compressed)
				return DXGI_FORMAT_BC4_UNORM;
			
			return DXGI_FORMAT_R8_UNORM;
		case 2:
			if (compressed)
				return DXGI_FORMAT_BC5_UNORM;
			
			return DXGI_FORMAT_R8G8_UNORM;
		case 3:
			if (compressed)
				return DXGI_FORMAT_BC1_UNORM;
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case 4:
			if (compressed)
				return DXGI_FORMAT_BC7_UNORM;
			
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		default:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
}

modelViewer::render::shader_texture_type texture_2D_dx::getType() const
{
	return modelViewer::render::shader_texture_type::texture2D;
}
