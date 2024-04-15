#include "texture_dx.h"
#include "../../resource/DDSTextureLoader.h"
#include "dx_util.h"
#include "../../common/string_util.h"

using namespace modelViewer::render::dx;

void texture_dx::uploadData(texture_setup& texture_setup, Microsoft::WRL::ComPtr<ID3D12Device>&
                            device,
                            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList)
{
    auto firstAsset = texture_setup.assets[0];
    if (firstAsset->getName().ends_with(".dds"))
    {
        auto fileName = convertAnsiToWide(texture_setup.assets[0]->getName());
        DirectX::CreateDDSTextureFromFile12(device.Get(), commandList.Get(), fileName.c_str(), m_Resource,
                                            m_UploadHeap);
        return;
    }

    createTextureFromRawData(texture_setup, device, commandList);
    auto wideName = convertAnsiToWide(firstAsset->getName());
    m_Resource->SetName(wideName.data());
}

D3D12_SHADER_RESOURCE_VIEW_DESC texture_dx::getView() const
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = m_Resource->GetDesc().Format;
    srvDesc.ViewDimension = getSRVDimension();
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = m_Resource->GetDesc().MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    return srvDesc;
}

ID3D12Resource* texture_dx::getResource() const
{
    return m_Resource.Get();
}

void texture_dx::createTextureFromRawData(texture_setup& setup,
                                          Microsoft::WRL::ComPtr<ID3D12Device>& device,
                                          Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList)
{
    int width = setup.assets[0]->getWidth();
    int height = setup.assets[0]->getHeight();
    auto format = getTextureFormat(setup.assets[0], false);
    // Create a texture resource
    CD3DX12_RESOURCE_DESC desc(
        getResourceDimension(),
        0,
        width,
        height,
        getArraySize(),
        setup.isMipMapActive ? 0 : 1,
        format,
        1,
        0,
        D3D12_TEXTURE_LAYOUT_UNKNOWN,
        D3D12_RESOURCE_FLAG_NONE
    );

    attempt(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&m_Resource)));

    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    subresources.reserve(setup.assets.size());
    for (const auto& asset: setup.assets)
    {
        D3D12_SUBRESOURCE_DATA subresource = {};
        subresource.pData = asset->getContent();
        auto channelCount = asset->getChannelCount();
        subresource.RowPitch = width * channelCount;
        subresource.SlicePitch = subresource.RowPitch * height;

        subresources.push_back(subresource);
    }

    // Transition the texture to a copy destination state
    auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);;
    UINT64 requiredSize = GetRequiredIntermediateSize(m_Resource.Get(), 0, 1) * subresources.size();

    attempt(device->CreateCommittedResource(
        &uploadHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(requiredSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_UploadHeap)));

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(),
                                                                          D3D12_RESOURCE_STATE_COMMON,
                                                                          D3D12_RESOURCE_STATE_COPY_DEST));
    // Copy the pixel data to the upload heap
    UpdateSubresources(commandList.Get(), m_Resource.Get(), m_UploadHeap.Get(), 0, 0, subresources.size(), subresources
                       .data());

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(),
                                                                          D3D12_RESOURCE_STATE_COPY_DEST,
                                                                          D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}

int texture_dx::getBytesPerPixel(const DXGI_FORMAT& format)
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
DXGI_FORMAT texture_dx::getTextureFormat(std::shared_ptr<res::texture_asset>& texture, bool compressed)
{
    int channels = texture->getChannelCount();
    switch (channels)
    {
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
