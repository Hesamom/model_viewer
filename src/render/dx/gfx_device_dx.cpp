#include "gfx_device_dx.h"
#include "../../resource/model_loader.h"
#include "mesh_dx.h"
#include <WindowsX.h>
#include <imgui/core/imgui.h>
#include <imgui/dx/imgui_impl_dx12.h>
#include "texture_cube_dx.h"
#include "texture_2D_dx.h"
#include "framebuffer_dx.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <WinPixEventRuntime/pix3.h>
#include "uniform_buffer_dx.h"

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#endif

using namespace Microsoft::WRL;
using namespace modelViewer::render::dx;
using namespace modelViewer::render;

void gfx_device_dx::initContext()
{
    enableDebugLayer(false);
    createDevice();
    createFence();
    queryDescriptorSizes();
    queryMSAASupport();
    createCommandList();
    //openCommandList();
    createSwapChain();

    //TODO use descriptor_heap class instead for better management 
    createDescriptorHeaps();
    createRenderTargets();
    createStencilDepthBuffer();
    createSRVHeap();

    setViewport(m_Window->getWidth(), m_Window->getHeight());
    setScissorRect(0, 0, m_Window->getWidth(), m_Window->getHeight());

    presentFrame();
    
    openCommandList();
}


void gfx_device_dx::createFence()
{
    attempt(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
}

void gfx_device_dx::createCommandList()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    attempt(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));
    attempt(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_DirectCmdListAlloc)));
    attempt(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                        m_DirectCmdListAlloc.Get(), nullptr, IID_PPV_ARGS(&m_CommandList)));
    m_IsCommandListOpen = true;
}

void gfx_device_dx::queryMSAASupport()
{
    //Need to study this later 
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = m_BackBufferFormat;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;
    attempt(m_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels,
                                          sizeof(msQualityLevels)));
    m_MsaaQuality = msQualityLevels.NumQualityLevels;

    //TODO how does this work?!
    assert(m_MsaaQuality > 0 && "Unexpected MSAA quality level.");
}

void gfx_device_dx::queryDescriptorSizes()
{
    mRtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mDsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void gfx_device_dx::createDevice()
{
    attempt(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory)));
    HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device));

    // Fallback to WARP device.
    if (FAILED(hardwareResult))
    {
        std::cerr << "Failed at creating dx12 hardware, fallback to WRAP \n";
        ComPtr<IDXGIAdapter> pWarpAdapter;
        m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter));
        attempt(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
    }
}

void gfx_device_dx::enableDebugLayer(bool enableGPULayer) const
{
#if defined(DEBUG) || defined(_DEBUG)
    ComPtr<ID3D12Debug> debugController;
    D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
    debugController->EnableDebugLayer();

    std::cout << "CPU Debug layer is enabled for dx\n";

    if (debugController != nullptr && enableGPULayer)
    {
        ComPtr<ID3D12Debug1> spDebugController1;
        debugController->QueryInterface(IID_PPV_ARGS(&spDebugController1));
        if (spDebugController1)
        {
            spDebugController1->SetEnableSynchronizedCommandQueueValidation(true);
            spDebugController1->SetEnableGPUBasedValidation(true);
            std::cout << "GPU Debug layer is enabled for dx\n";
        }
    }

#endif
}


void gfx_device_dx::createRenderTargets()
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < SwapChainBufferCount; i++)
    {
        m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i]));
        m_device->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
        rtvHeapHandle.Offset(1, mRtvDescriptorSize);
    }
}

void gfx_device_dx::createSRVHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = 256;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    attempt(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_SRVHeap)));
}

void gfx_device_dx::setScissorRect(int x, int y, int width, int height)
{
    mScissorRect.left = x;
    mScissorRect.top = y;
    mScissorRect.right = x + width;
    mScissorRect.bottom = y + height;
}

void gfx_device_dx::setViewport(int width, int height)
{
    ZeroMemory(&mScreenViewport, sizeof(D3D12_VIEWPORT));
    mScreenViewport.MaxDepth = 1.0f;
    mScreenViewport.Width = static_cast<float>(width);
    mScreenViewport.Height = static_cast<float>(height);
}

void gfx_device_dx::createStencilDepthBuffer()
{
    // Create the depth/stencil buffer and view. D3D12_RESOURCE_DESC depthStencilDesc;
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = m_Window->getWidth();
    depthStencilDesc.Height = m_Window->getHeight();
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = mDepthStencilFormat;
    depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
    depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m_MsaaQuality - 1) : 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    D3D12_CLEAR_VALUE optClear;
    optClear.Format = mDepthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    m_device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilDesc,
                                      D3D12_RESOURCE_STATE_COMMON, &optClear,
                                      IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf()));

    // Create descriptor to mip level 0 of entire resource using the format of the resource.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = mDepthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;
    m_device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, getDepthStencilView());

    // Transition the resource from its initial state to be used as a depth buffer.
    //auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(),D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_PRESENT);
    //m_CommandList->ResourceBarrier(1, &barrier);
}

void gfx_device_dx::createSwapChain()
{
    m_SwapChain.Reset();
    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = m_Window->getWidth();
    sd.BufferDesc.Height = m_Window->getHeight();

    //TODO check what refreshRate entails here 
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = m_BackBufferFormat;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    //TODO DXGI_SWAP_EFFECT_FLIP_DISCARD does not support multi sampling!
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = SwapChainBufferCount;
    sd.OutputWindow = m_Window->getHandle();
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    attempt(m_dxgiFactory->CreateSwapChain(m_CommandQueue.Get(), &sd, m_SwapChain.GetAddressOf()));
}


void gfx_device_dx::createDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    attempt(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap)));

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    attempt(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap)));
}

D3D12_CPU_DESCRIPTOR_HANDLE gfx_device_dx::getCurrentBackBufferView() const
{
    // CD3DX12 constructor to offset to the RTV of the current back buffer. 
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrBackBuffer,
                                         mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE gfx_device_dx::getDepthStencilView() const
{
    return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void gfx_device_dx::openCommandList()
{
    if (m_IsCommandListOpen)
    {
        return;
    }

    attempt(m_DirectCmdListAlloc->Reset());
    attempt(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));
    
    m_IsCommandListOpen = true;
}

void gfx_device_dx::closeCommandList()
{
    if (!m_IsCommandListOpen)
    {
        return;
    }

    attempt(m_CommandList->Close());
    ID3D12CommandList* cmdsLists[] = {m_CommandList.Get()};
    m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
    m_IsCommandListOpen = false;
}


void gfx_device_dx::onEndRender()
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(getCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                        D3D12_RESOURCE_STATE_PRESENT);
    auto barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE,
                                                        D3D12_RESOURCE_STATE_PRESENT);
    CD3DX12_RESOURCE_BARRIER barriers[2] = {barrier, barrier2};
    m_CommandList->ResourceBarrier(2, barriers);
    presentFrame();
}

void gfx_device_dx::presentFrame()
{
    closeCommandList();
    attempt(m_SwapChain->Present(m_Vsync, 0));
    mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
    flushCommandQueue();
}


void gfx_device_dx::onStartRender()
{
    openCommandList();
    m_CommandList->RSSetViewports(1, &mScreenViewport);
    m_CommandList->RSSetScissorRects(1, &mScissorRect);
    
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(getCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT,
                                                        D3D12_RESOURCE_STATE_RENDER_TARGET);
    auto barrierDepth = CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    
    CD3DX12_RESOURCE_BARRIER barriers[2] = {barrier, barrierDepth};
    m_CommandList->ResourceBarrier(2, barriers);

    auto backBufferView = getCurrentBackBufferView();
    auto depthBufferView = getDepthStencilView();
    m_CommandList->OMSetRenderTargets(1, &backBufferView, true, &depthBufferView);
    shader_program_dx::clearHeap();
    shader_program_dx::setGPUHeap(m_CommandList);
}


void gfx_device_dx::flushCommandQueue()
{
    // Advance the fence value to mark commands up to this fence point.
    mCurrentFence++;

    // Add an instruction to the command queue to set a new fence point.  Because we 
    // are on the GPU timeline, the new fence point won't be set until the GPU finishes
    // processing all the commands prior to this Signal().
    m_CommandQueue->Signal(m_Fence.Get(), mCurrentFence);

    // Wait until the GPU has completed commands up to this fence point.
    if (m_Fence->GetCompletedValue() < mCurrentFence)
    {
        //TODO should be able to resue this event handle 
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
        // Fire event when GPU hits current fence.  
        m_Fence->SetEventOnCompletion(mCurrentFence, eventHandle);
        // Wait until the GPU hits current fence event is fired.
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}


void gfx_device_dx::resize(int width, int height)
{
    assert(m_device);
    assert(m_SwapChain);
    assert(m_DirectCmdListAlloc);
    
    flushCommandQueue();

    // Release the previous resources we will be recreating.
    for (auto& i: m_SwapChainBuffer)
    {
        i.Reset();
    }

    m_DepthStencilBuffer.Reset();
    // Resize the swap chain.
    m_SwapChain->ResizeBuffers(SwapChainBufferCount, width, height,
                               m_BackBufferFormat,
                               DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
    mCurrBackBuffer = 0;

    createRenderTargets();
    createStencilDepthBuffer();
    presentFrame();
    setViewport(width, height);
}


void gfx_device_dx::clearDepthBuffer()
{
    m_CommandList->ClearDepthStencilView(getDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f,
                                         0, 0, nullptr);
}

void gfx_device_dx::clearColorBuffer(const glm::vec4& color)
{
    float clearColor[4] = {color.r, color.g, color.b, 1.0f};
    m_CommandList->ClearRenderTargetView(getCurrentBackBufferView(), clearColor, 0, nullptr);
}

void* gfx_device_dx::getDevice()
{
    return m_device.Get();
}

void gfx_device_dx::popDebugGroup()
{
    PIXEndEvent(m_CommandList.Get());
}

void gfx_device_dx::pushDebugGroup(const char* label)
{
    auto color = PIX_COLOR(1, 1, 1);
    PIXBeginEvent(m_CommandList.Get(), color, label);
}

int gfx_device_dx::getMaxSamplersPerProgram()
{
    return 0;
}

gfx_device_dx::gfx_device_dx(std::shared_ptr<window_win32>& window)
{
    m_Window = window;
    initContext();
}

std::shared_ptr<texture> gfx_device_dx::createTextureCube(texture_setup& setup)
{
    auto texture = std::make_shared<texture_cube_dx>(setup, m_device, m_CommandList);
    return texture;
}

std::shared_ptr<texture> gfx_device_dx::createTexture2D(texture_setup& setup)
{
    auto texture = std::make_shared<texture_2D_dx>(setup, m_device, m_CommandList);
    return texture;
}

std::shared_ptr<mesh> gfx_device_dx::createMesh(
    std::shared_ptr<res::mesh_asset>& asset)
{
    auto mesh = std::make_shared<mesh_dx>(asset, m_device, m_CommandList);
    return mesh;
}

std::shared_ptr<shader_program> gfx_device_dx::createProgram(std::vector<std::shared_ptr<res::shader_asset>>& assets)
{
    std::vector<std::shared_ptr<shader_dx>> shaders;
    for (auto& asset: assets)
    {
        auto shader = std::make_shared<shader_dx>(asset);
        shader->compileToByteCode();
        shaders.push_back(shader);
    }

    auto program = std::make_shared<shader_program_dx>(shaders, m_device, m_CommandList);
    return program;
}

std::shared_ptr<framebuffer> gfx_device_dx::createFramebuffer(std::string& name)
{
    auto frameBuffer = std::make_shared<framebuffer_dx>(m_device, m_CommandList, name);
    return frameBuffer;
}

std::shared_ptr<uniform_buffer> gfx_device_dx::createUniformBuffer(int size, std::string& name)
{
    auto buffer = std::make_shared<uniform_buffer_dx>(*m_device.Get(), size, name.data());
    return buffer;
}

void gfx_device_dx::onPreRenderImGUI()
{
    openCommandList();
    ImGui_ImplDX12_NewFrame();
}

void gfx_device_dx::onPostRenderImGUI()
{
    ID3D12DescriptorHeap* heaps[] = {m_SRVHeap.Get()};
    m_CommandList->SetDescriptorHeaps(1, heaps);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_CommandList.Get());
}

void gfx_device_dx::onInitImGUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    m_Window->onInitImGUI();

    ImGui_ImplDX12_Init(m_device.Get(), SwapChainBufferCount, m_BackBufferFormat,
                        m_SRVHeap.Get(), m_SRVHeap->GetCPUDescriptorHandleForHeapStart(),
                        m_SRVHeap->GetGPUDescriptorHandleForHeapStart());
}

void gfx_device_dx::onShutdownImGUI()
{
    ImGui_ImplDX12_Shutdown();
}

ID3D12Resource* gfx_device_dx::getCurrentBackBuffer()
{
    return m_SwapChainBuffer[mCurrBackBuffer].Get();
}
