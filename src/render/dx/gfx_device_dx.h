#ifndef GFX_DEVICE_DX_H
#define GFX_DEVICE_DX_H
#include "../gfx_device.h"

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include "d3dx12.h"
#include "../../window/window_win32.h"
#include "../texture.h"
#include "../../resource/model_info.h"

namespace modelViewer::render
{
	class gfx_device_dx : public gfx_device {

	public:
		explicit gfx_device_dx(std::shared_ptr<window_win32>& window);

		void swapBuffers() override;
		void setViewport(int width, int height) override;
		void setClearColor(glm::vec4& color) override;
		void resize(int width, int height) override;
		void setCullFaceMode(cull_face_mode mode) override;
		void setDepthmap(bool enable) override;
		void setCullFace(bool enable) override;
		void clearDepthBuffer() override;
		void clearColorBuffer(const glm::vec4& color) override;
		void* getDevice() override;
		void popDebugGroup() override;
		void pushDebugGroup(const char* label) override;
		
		int getMaxSamplersPerProgram() override;

		std::shared_ptr<texture> createTextureCube(texture_setup &setup) override ;
		std::shared_ptr<texture> createTexture2D(texture_setup &setup) override ;
		std::shared_ptr<mesh> createMesh(std::shared_ptr<res::mesh_asset>& asset) override;
		std::shared_ptr<shader_program> createProgram(std::vector<std::shared_ptr<res::shader_asset>>& assets) override;
		std::shared_ptr<framebuffer> createFramebuffer() override;

	private:

		void initDevice();

		void createDescriptorHeaps();

		void createSwapChain();

		void createRenderTargets();

		void createStencilDepthBuffer();

		void flushCommandQueue();
		
		D3D12_CPU_DESCRIPTOR_HANDLE getCurrentBackBufferView() const;

		D3D12_CPU_DESCRIPTOR_HANDLE getDepthStencilView() const;

		std::shared_ptr<window_win32> m_Window;

		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
		int mCurrentFence;
		
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;

		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;
		

		static constexpr int SwapChainBufferCount = 2;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[SwapChainBufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;


		const DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		const DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	
		UINT mCurrBackBuffer = 0;
		bool m4xMsaaState = false;
		UINT m4xMsaaQuality = 0;

	};
}

#endif //GFX_DEVICE_DX_H
