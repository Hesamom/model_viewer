﻿
#include "gfx_device_dx.h"
#include "../../error/not_implemented_error.h"
#include <WindowsX.h>
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

using namespace Microsoft::WRL;
using namespace modelViewer::render;

void gfx_device_dx::initDevice()
{
	//TODO check when we define "DEBUG" in cmake 
#if defined(DEBUG) || defined(_DEBUG)
	std::cout << "debug layer is enable for dx\n";
	// Enable the D3D12 debug layer.
	ComPtr<ID3D12Debug> debugController;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
	debugController->EnableDebugLayer();
#endif

	CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice( nullptr,D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice));

	// Fallback to WARP device.
	if (FAILED(hardwareResult)) {
		std::cerr<< "failed at creating dx12 hardware, fallback to WRAP \n";
		ComPtr<IDXGIAdapter> pWarpAdapter;
		m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter));
		D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice));
	}

	m_d3dDevice->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));

	mRtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//Need to study this later 
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = m_BackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	m_d3dDevice->CheckFeatureSupport( D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels,sizeof(msQualityLevels));
	m4xMsaaQuality = msQualityLevels.NumQualityLevels;

	//TODO how does this work?!
	assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");



	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	m_d3dDevice->CreateCommandQueue( &queueDesc, IID_PPV_ARGS(&mCommandQueue));
	m_d3dDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_DirectCmdListAlloc));
	m_d3dDevice->CreateCommandList( 0,D3D12_COMMAND_LIST_TYPE_DIRECT,m_DirectCmdListAlloc.Get(), nullptr, IID_PPV_ARGS(&m_CommandList));

	m_CommandList->Close();

	createSwapChain();
	createDescriptorHeaps();
	createRenderTargets();
	createStencilDepthBuffer();
	setViewport(m_Window->getWidth(), m_Window->getHeight());
}


void gfx_device_dx::createRenderTargets()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle( mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++) {
		m_SwapChain->GetBuffer( i, IID_PPV_ARGS(&m_SwapChainBuffer[i]));
		m_d3dDevice->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}
}

void gfx_device_dx::setViewport(int width, int height)
{
	D3D12_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_CommandList->RSSetViewports(1, &vp);

	tagRECT mScissorRect = { 0, 0, width/2, height/2 };
	m_CommandList->RSSetScissorRects(1, &mScissorRect);
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
	depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	m_d3dDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON, &optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf()));

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	m_d3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, getDepthStencilView());

	// Transition the resource from its initial state to be used as a depth buffer.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

}

void gfx_device_dx::createSwapChain() {

	m_SwapChain.Reset();
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = m_Window->getWidth();
	sd.BufferDesc.Height =  m_Window->getHeight();
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = m_BackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow =  m_Window->getHandle();
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// Note: Swap chain uses queue to perform flush. 

	m_dxgiFactory->CreateSwapChain( mCommandQueue.Get(), &sd,m_SwapChain.GetAddressOf());
}


void gfx_device_dx::createDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	m_d3dDevice->CreateDescriptorHeap( &rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	m_d3dDevice->CreateDescriptorHeap( &dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap));
}

D3D12_CPU_DESCRIPTOR_HANDLE gfx_device_dx::getCurrentBackBufferView() const
{
	// CD3DX12 constructor to offset to the RTV of the current back buffer. 
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrBackBuffer, mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE gfx_device_dx::getDepthStencilView() const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void gfx_device_dx::swapBuffers()
{
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_SwapChainBuffer[mCurrBackBuffer].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	m_SwapChain->Present(0,0);
	mCurrBackBuffer++;
	if (mCurrBackBuffer > 1)
		mCurrBackBuffer = 0;
}

void gfx_device_dx::setClearColor(glm::vec4& color)
{
	
}

void gfx_device_dx::flushCommandQueue()
{
	// Advance the fence value to mark commands up to this fence point.
	mCurrentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	mCommandQueue->Signal(m_Fence.Get(), mCurrentFence);

	// Wait until the GPU has completed commands up to this fence point.
	if(m_Fence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		// Fire event when GPU hits current fence.  
		m_Fence->SetEventOnCompletion(mCurrentFence, eventHandle);
		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}


void gfx_device_dx::resize(int width, int height) {
	
	assert(m_d3dDevice);
	assert(m_SwapChain);
    assert(m_DirectCmdListAlloc);

	// Flush before changing any resources.
	flushCommandQueue();

    m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr);

	// Release the previous resources we will be recreating.
	for (auto & i : m_SwapChainBuffer) {
		i.Reset();
	}
	
    mDepthStencilBuffer.Reset();
	// Resize the swap chain.
    m_SwapChain->ResizeBuffers(SwapChainBufferCount, width, height, 
		m_BackBufferFormat, 
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	mCurrBackBuffer = 0;

	createRenderTargets();
	createStencilDepthBuffer();
	
    // Execute the resize commands.
    m_CommandList->Close();
	
    ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	flushCommandQueue();
	setViewport(width, height);
}

void gfx_device_dx::setCullFaceMode(cull_face_mode mode) {
}

void gfx_device_dx::setDepthmap(bool enable) {
}

void gfx_device_dx::setCullFace(bool enable) {
}

void gfx_device_dx::clearDepthBuffer() {
}

void gfx_device_dx::clearColorBuffer(const glm::vec4& color)
{
	const auto handle = getCurrentBackBufferView();
	
	m_CommandList->OMSetRenderTargets(1, &handle, FALSE, nullptr);
	m_CommandList->ClearRenderTargetView(handle, &color.x, 0, nullptr);
}

void* gfx_device_dx::getDevice() {
	return m_d3dDevice.Get();
}

void gfx_device_dx::popDebugGroup() {
}

void gfx_device_dx::pushDebugGroup(const char* label) {
}

int gfx_device_dx::getMaxSamplersPerProgram() {
	return 0;
}

gfx_device_dx::gfx_device_dx(std::shared_ptr<window_win32>& window)
{
	m_Window = window;
	initDevice();
}

std::shared_ptr<texture> gfx_device_dx::createTextureCube(texture_setup& setup)
{
	return std::shared_ptr<texture>();
}

std::shared_ptr<texture> gfx_device_dx::createTexture2D(texture_setup& setup)
{
	return std::shared_ptr<texture>();
}

std::shared_ptr<mesh> gfx_device_dx::createMesh(
	std::shared_ptr<res::mesh_asset>& asset) {
	throw std::runtime_error("not imp");
}

std::shared_ptr<shader_program> gfx_device_dx::createProgram(std::vector<std::shared_ptr<res::shader_asset>>& assets) {
	throw std::runtime_error("not imp");
}

std::shared_ptr<framebuffer> gfx_device_dx::createFramebuffer() {
	throw std::runtime_error("not imp");
}