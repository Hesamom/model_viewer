
#include "gfx_device_dx.h"
#include "../../error/not_implemented_error.h"
#include "../../resource/model_loader.h"
#include "mesh_dx.h"
#include "buffer_constant_dx.h"
#include <WindowsX.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include "../../resource/texture_loader.h"
#include "texture_cube_dx.h"
#include "texture_2D_dx.h"

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#endif

using namespace Microsoft::WRL;
using namespace modelViewer::render;


void gfx_device_dx::initContext()
{
	enableDebugLayer();
	createDevice();
	createFence();
	queryDescriptorSizes();
	initMSAA();
	createCommandList();
	createSwapChain();
	createDescriptorHeaps();
	createRenderTargets();
	createStencilDepthBuffer();
	createSRVHeap();
	
	//for testing
	//createShaderSamples();
	createSkyboxShaderSamples();
	createSampleGeometry();
	
	setViewport(m_Window->getWidth(), m_Window->getHeight());
	setScissorRect(0,0, m_Window->getWidth(), m_Window->getHeight());
	
	m_CommandList->Close();
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	m_SwapChain->Present(0,0);
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	flushCommandQueue();
	
}

void gfx_device_dx::createSampleGeometry()
{
	modelViewer::res::model_loader loader;
	auto meshAsset = loader.loadFirstMesh(modelViewer::res::literals::models::primitive_cube);

	std::shared_ptr<std::vector<glm::vec4>> colors = std::make_shared<std::vector<glm::vec4>>();
	for (int i = 0; i < meshAsset->positions->size(); i++) {
		colors->emplace_back(1,1,1,1);
	}
	meshAsset->colors = colors;
	
	for (int i = 0; i < meshCount; ++i) {
		auto mesh = std::make_unique<mesh_dx>(meshAsset, m_device, m_CommandList);
		auto program = std::static_pointer_cast<shader_program>(m_SamplePrograms[i]);
		mesh->bindLayout(program);
		m_sampleMeshes.push_back(std::move(mesh));
	}
}

void gfx_device_dx::createFence()
{ m_device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)); }

void gfx_device_dx::createCommandList()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	m_device->CreateCommandQueue( &queueDesc, IID_PPV_ARGS(&m_CommandQueue));
	m_device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_DirectCmdListAlloc));
	m_device->CreateCommandList( 0,D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_DirectCmdListAlloc.Get(), nullptr, IID_PPV_ARGS(&m_CommandList));
	//m_CommandList->re();
}

void gfx_device_dx::initMSAA()
{
	//Need to study this later 
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = m_BackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	m_device->CheckFeatureSupport( D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels,sizeof(msQualityLevels));
	m4xMsaaQuality = msQualityLevels.NumQualityLevels;

	//TODO how does this work?!
	assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");
}

void gfx_device_dx::queryDescriptorSizes()
{
	mRtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void gfx_device_dx::createDevice()
{
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory));
	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice( nullptr,D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));

	// Fallback to WARP device.
	if (FAILED(hardwareResult)) {
		std::cerr<< "failed at creating dx12 hardware, fallback to WRAP \n";
		ComPtr<IDXGIAdapter> pWarpAdapter;
		m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter));
		D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
	}
}

void gfx_device_dx::enableDebugLayer() const
{
	
	//TODO check when we define "DEBUG" in cmake 
#if defined(DEBUG) || defined(_DEBUG)
	std::cout << "debug layer is enable for dx\n";
	// Enable the D3D12 debug layer.
	ComPtr<ID3D12Debug> debugController;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
	debugController->EnableDebugLayer();

	if (debugController != nullptr)
	{
		ComPtr<ID3D12Debug1> spDebugController1;
		debugController->QueryInterface(IID_PPV_ARGS(&spDebugController1));
		if(spDebugController1)
		{
			spDebugController1->SetEnableSynchronizedCommandQueueValidation(true);
			spDebugController1->SetEnableGPUBasedValidation(true);
		}
	}
	
#endif

}


void gfx_device_dx::createRenderTargets()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle( mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++) {
		m_SwapChain->GetBuffer( i, IID_PPV_ARGS(&m_SwapChainBuffer[i]));
		m_device->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}
}

void gfx_device_dx::createSRVHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_SRVHeap));
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
	depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	m_device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON, &optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf()));

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	m_device->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, getDepthStencilView());

	// Transition the resource from its initial state to be used as a depth buffer.
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	m_CommandList->ResourceBarrier(1, &barrier);

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

	m_dxgiFactory->CreateSwapChain( m_CommandQueue.Get(), &sd,m_SwapChain.GetAddressOf());
}


void gfx_device_dx::createDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	m_device->CreateDescriptorHeap( &rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	m_device->CreateDescriptorHeap( &dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap));
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
	dx::shader_program_dx::clearHeap();
	dx::shader_program_dx::setGPUHeap(m_CommandList);
	
	auto view = glm::lookAt(
		glm::vec3(0,0,-5),
		glm::vec3(0),
		glm::vec3(0,1,0));
	
	auto width = m_Window->getWidth();
	auto height = m_Window->getHeight();
	
	auto aspectRatio = (float)width / height;
	float fov = 90;
	auto projection = glm::perspective<float>(glm::radians(fov),aspectRatio,0.1f,100);
	
	glm::vec4  colors[4] = {
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)
	};
	modelViewer::common::transform transform;
	transform.setPosition(glm::vec3(0, 0, 0));
	transform.setScale(glm::vec3(1.0));
	
	for (auto i = 0; i < 2; ++i)
	{
		auto model = transform.getMatrix();
		auto mvp = projection * view * model;
		
		//auto tMvp = transpose(mvp);
		auto program = m_SamplePrograms[i];
		program->setUniform("gWorldViewProj", mvp, true);
		program->bind();

		m_sampleMeshes[i]->bind();
		m_sampleMeshes[i]->draw();

		auto pos = transform.getPosition();
		pos.x += 3;
		pos.y += 2;
		transform.setPosition(pos);
	}
	
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(getCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_CommandList->ResourceBarrier(1, &barrier);
	
	
	m_CommandList->Close();
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	
	m_SwapChain->Present(0,0);
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
	
	flushCommandQueue();
}

void gfx_device_dx::onStartRender()
{
	m_DirectCmdListAlloc->Reset();
	m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr);

	m_CommandList->RSSetViewports(1, &mScreenViewport);
	m_CommandList->RSSetScissorRects(1, &mScissorRect);

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(getCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_CommandList->ResourceBarrier(1, &barrier);

	auto backBufferView = getCurrentBackBufferView();
	auto depthBufferView = getDepthStencilView();
	m_CommandList->OMSetRenderTargets(1, &backBufferView, true, &depthBufferView);
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
	
	assert(m_device);
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
    m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	flushCommandQueue();
	setViewport(width, height);
}


void gfx_device_dx::clearDepthBuffer()
{
	m_CommandList->ClearDepthStencilView(getDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void gfx_device_dx::clearColorBuffer(const glm::vec4& color)
{
	float clearColor[4] = { color.r, color.g, color.b, 1.0f };
	m_CommandList->ClearRenderTargetView(getCurrentBackBufferView(), clearColor, 0, nullptr);
}

void* gfx_device_dx::getDevice() {
	return m_device.Get();
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
	initContext();
}

std::shared_ptr<texture> gfx_device_dx::createTextureCube(texture_setup& setup)
{
	auto texture = std::make_shared<dx::texture_cube_dx>(setup, m_device, m_CommandList);
	return texture;
}

std::shared_ptr<texture> gfx_device_dx::createTexture2D(texture_setup& setup)
{
	auto texture = std::make_shared<dx::texture_2D_dx>(setup, m_device, m_CommandList);
	return texture;
}

std::shared_ptr<mesh> gfx_device_dx::createMesh(
	std::shared_ptr<res::mesh_asset>& asset) {
	
	auto mesh = std::make_shared<mesh_dx>(asset, m_device, m_CommandList);
	return mesh;
}

std::shared_ptr<shader_program> gfx_device_dx::createProgram(std::vector<std::shared_ptr<res::shader_asset>>& assets) {

	std::vector<std::shared_ptr<shader_dx>> shaders;
	for (auto& asset : assets) {
		auto shader = std::make_shared<shader_dx>(asset);
		shader->compileToByteCode();
		shaders.push_back(shader);
	}
	
	auto program = std::make_shared<dx::shader_program_dx>(shaders, m_device, m_CommandList);
	return program;
}

std::shared_ptr<framebuffer> gfx_device_dx::createFramebuffer() {
	throw std::runtime_error("not imp");
}

void gfx_device_dx::onPreRenderImGUI()
{
	ImGui_ImplDX12_NewFrame();
}

void gfx_device_dx::onPostRenderImGUI()
{
	m_CommandList->SetDescriptorHeaps(1, &m_SRVHeap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_CommandList.Get());
}

void gfx_device_dx::onInitImGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
	
	m_Window->onInitImGUI();
	//BUG not sure about the second arg 
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

void gfx_device_dx::createShaderSamples()
{
	texture_setup setup;
	res::textureInfo info;
	res::texture_loader tloader;
	auto asset = tloader.load(res::literals::textures::uv_checker, 4, false);
	setup.assets.push_back(asset);
	
	m_SampleTexture = std::make_shared<dx::texture_2D_dx>(setup, m_device, m_CommandList);
	res::shader_loader loader;
	
	auto fragAsset = loader.load("Shaders/texture.hlsl", res::shaderType::fragment);
	auto vertAsset = loader.load("Shaders/texture.hlsl", res::shaderType::vertex);

	for (int i = 0; i < meshCount; ++i) {

		auto frag = std::make_shared<shader_dx>(fragAsset);
		frag->compileToByteCode();
		auto vert = std::make_shared<shader_dx>(vertAsset);
		vert->compileToByteCode();

		std::vector<std::shared_ptr<shader_dx>> shaders = {vert,frag};
		auto program = std::make_shared<dx::shader_program_dx>(shaders, m_device, m_CommandList);
		auto text = std::shared_ptr<texture>(m_SampleTexture);
		program->bindTexture(0, text);
		m_SamplePrograms.push_back(program);
	}

}

void gfx_device_dx::createSkyboxShaderSamples()
{
	res::texture_asset_info skyboxTexture;
	skyboxTexture.type = res::texture_asset_type::textureCube;
	skyboxTexture.paths.emplace_back(res::literals::textures::skybox_right);
	skyboxTexture.paths.emplace_back(res::literals::textures::skybox_left);
	skyboxTexture.paths.emplace_back(res::literals::textures::skybox_top);
	skyboxTexture.paths.emplace_back(res::literals::textures::skybox_bottom);
	skyboxTexture.paths.emplace_back(res::literals::textures::skybox_front);
	skyboxTexture.paths.emplace_back(res::literals::textures::skybox_back);
	texture_setup setup;
	setup.type = skyboxTexture.type;
	res::texture_loader tloader;
	for (auto& path: skyboxTexture.paths) {
		auto asset = tloader.load(path, 4, false);
		setup.assets.push_back(asset);
	}


	m_SkyBoxTexture = createTextureCube(setup);
	res::shader_loader loader;

	auto fragAsset = loader.load("Shaders/texture_cube.hlsl", res::shaderType::fragment);
	auto vertAsset = loader.load("Shaders/texture_cube.hlsl", res::shaderType::vertex);

	for (int i = 0; i < meshCount; ++i) {

		auto frag = std::make_shared<shader_dx>(fragAsset);
		frag->compileToByteCode();
		auto vert = std::make_shared<shader_dx>(vertAsset);
		vert->compileToByteCode();

		std::vector<std::shared_ptr<shader_dx>> shaders = {vert,frag};
		auto program = std::make_shared<dx::shader_program_dx>(shaders, m_device, m_CommandList);
		auto text = std::shared_ptr<texture>(m_SkyBoxTexture);
		program->bindTexture(0, text);
		m_SamplePrograms.push_back(program);
	}
}




