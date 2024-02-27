
#include "window_dx12.h"
#include <WindowsX.h>
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif



using namespace Microsoft::WRL;

window_dx12::window_dx12(int width, int height, const std::string& title, bool fullscreen)
{
	//TODO add validations 
	m_Width = width;
	m_Height = height;
	m_Instance = GetModuleHandle(nullptr);

	if (!createWindow())
	{
		throw std::runtime_error("failed to create window!" + std::to_string(GetLastError()));
	}
	
	initDevice();
}

void window_dx12::initDevice()
{
	//TODO
	// 0. Create window [done]
	// 1. Create the ID3D12Device using the D3D12CreateDevice function. [done]
	// 2. Create an ID3D12Fence object and query descriptor sizes. [done]
	// 3. Check 4X MSAA quality level support. [done]
	// 4. Create the command queue, command list allocator, and main command list. [done]
	// 5. Describe and create the swap chain. [done]
	// 6. Create the descriptor heaps the application requires. [done]
	// 7. Resize the back buffer and create a render target view to the back buffer.  [done]
	// 8. Create the depth/stencil buffer and its associated depth/stencil view. [done]
	// 9. Set the viewport and scissor rectangles. [done]

	//TODO check when we define "DEBUG" in cmake 
	#if defined(DEBUG) || defined(_DEBUG) 
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
	m_d3dDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mDirectCmdListAlloc));
	m_d3dDevice->CreateCommandList( 0,D3D12_COMMAND_LIST_TYPE_DIRECT,mDirectCmdListAlloc.Get(), nullptr, IID_PPV_ARGS(&mCommandList));
		
	mCommandList->Close();
	
	createSwapChain();
	createDescriptorHeaps();
	createRenderTargets();
	createStencilDepthBuffer();
	setViewport();
}


void window_dx12::createSwapChain() {
 
	mSwapChain.Reset();
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = m_Width; 
	sd.BufferDesc.Height = m_Height;
	sd.BufferDesc.RefreshRate.Numerator = 60; 
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = m_BackBufferFormat; 
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0; 
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount; 
	sd.OutputWindow = mhMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; 
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// Note: Swap chain uses queue to perform flush. 
	
	m_dxgiFactory->CreateSwapChain( mCommandQueue.Get(), &sd,mSwapChain.GetAddressOf()); 
}


void window_dx12::createRenderTargets()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle( mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++) {
		mSwapChain->GetBuffer( i, IID_PPV_ARGS(&m_SwapChainBuffer[i]));
		m_d3dDevice->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle); 
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}
}

void window_dx12::setViewport()
{
	D3D12_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(m_Width);
	vp.Height = static_cast<float>(m_Height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	mCommandList->RSSetViewports(1, &vp);

	tagRECT mScissorRect = { 0, 0, m_Width/2, m_Height/2 };
	mCommandList->RSSetScissorRects(1, &mScissorRect);
}

void window_dx12::createStencilDepthBuffer()
{
	// Create the depth/stencil buffer and view. D3D12_RESOURCE_DESC depthStencilDesc;
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = m_Width;
	depthStencilDesc.Height = m_Height; 
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
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	
}


void window_dx12::createDescriptorHeaps()
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

D3D12_CPU_DESCRIPTOR_HANDLE window_dx12::getCurrentBackBufferView() const
{
	// CD3DX12 constructor to offset to the RTV of the current back buffer. 
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrBackBuffer, mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE window_dx12::getDepthStencilView() const 
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool window_dx12::createWindow()
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_Instance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc) )
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, m_Width, m_Height };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(L"MainWnd", mMainWndCaption.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, m_Instance, nullptr);
	if (!mhMainWnd)
	{
		MessageBox(nullptr, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

	return true;
}

void window_dx12::setSize(int width, int height)
{

}

int window_dx12::getHeight()
{
	return 0;
}

int window_dx12::getWidth()
{
	return 0;
}

int window_dx12::getTargetFrameRate()
{
	return 0;
}

void window_dx12::setTargetFrameRate(int fps)
{

}

void window_dx12::setTitle(std::string title)
{

}

void window_dx12::setVsync(bool enabled)
{

}

std::string window_dx12::getTitle()
{
	return std::string();
}

void window_dx12::draw()
{
	while (true)
	{
		
	}
}

window_dx12::~window_dx12()
{

}
