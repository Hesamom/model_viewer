#ifndef GFX_DEVICE_DX_H
#define GFX_DEVICE_DX_H

#include "../gfx_device.h"

#include <windows.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include "../../window/dx/window_win32.h"
#include "../texture.h"
#include "shader_dx.h"
#include "mesh_dx.h"
#include "shader_program_dx.h"

namespace modelViewer::render::dx
{
	class gfx_device_dx : public gfx_device {

	public:
		explicit gfx_device_dx(std::shared_ptr<window_win32>& window);
		
		void onEndRender() override;
		void onStartRender() override;
		void presentFrame();

	
		void setViewport(int width, int height) override;
		void resize(int width, int height) override;
		
		void clearDepthBuffer() override;
		void clearColorBuffer(const glm::vec4& color) override;
		void* getDevice() override;
		
		void onPreRenderImGUI() override;
		void onPostRenderImGUI() override;
		void onInitImGUI() override;
		void onShutdownImGUI() override;
		
		void popDebugGroup() override;
		void pushDebugGroup(const char* label) override;
		
		int getMaxSamplersPerProgram() override;

		std::shared_ptr<texture> createTextureCube(texture_setup &setup) override ;
		std::shared_ptr<texture> createTexture2D(texture_setup &setup) override ;
		std::shared_ptr<mesh> createMesh(std::shared_ptr<res::mesh_asset>& asset) override;
		std::shared_ptr<shader_program> createProgram(std::vector<std::shared_ptr<res::shader_asset>>& assets) override;
		std::shared_ptr<framebuffer> createFramebuffer(std::string& name) override;
		std::shared_ptr<uniform_buffer> createUniformBuffer(int size, std::string& name) override;

	private:
		void openCommandList();
		void closeCommandList();
		void initContext();
		void createDescriptorHeaps();
		void createSwapChain();
		void createRenderTargets();
		void createStencilDepthBuffer();
		void flushCommandQueue();
		void setScissorRect(int x, int y, int width, int height);
		void enableDebugLayer(bool enableGPULayer) const;
		void createDevice();
		void queryDescriptorSizes();
		void queryMSAASupport();
		void createCommandList();
		void createFence();
		D3D12_CPU_DESCRIPTOR_HANDLE getCurrentBackBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE getDepthStencilView() const;
		
		void createSRVHeap();
		ID3D12Resource* getCurrentBackBuffer();

		int m_Vsync = 0;
		std::vector<std::shared_ptr<dx::shader_program_dx>> m_SamplePrograms;
		std::vector<std::unique_ptr<mesh_dx>> m_sampleMeshes;
		std::shared_ptr<window_win32> m_Window;
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D12Device> m_device;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
		int mCurrentFence;
		
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;

		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRVHeap;

		static constexpr int SwapChainBufferCount = 2;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[SwapChainBufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;

		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;


		const DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		const DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;
	
		UINT mCurrBackBuffer = 0;
		bool m4xMsaaState = false;
		bool m_IsCommandListOpen = false;
		UINT m_MsaaQuality = 0;
		std::shared_ptr<dx::texture_dx> m_SampleTexture;
		std::shared_ptr<texture> m_SkyBoxTexture;
		
	};
}

#endif //GFX_DEVICE_DX_H
