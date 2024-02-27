
#ifndef WINDOW_DX12_H
#define WINDOW_DX12_H


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
#include "window.h"

class window_dx12 : public window{
	
private:
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	static constexpr int SwapChainBufferCount = 2;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

	std::wstring mMainWndCaption = L"d3d App";
	HWND  mhMainWnd = nullptr;
	HINSTANCE m_Instance = nullptr;

	const DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	const DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	UINT mCurrBackBuffer = 0;
	bool m4xMsaaState = false;
	UINT m4xMsaaQuality = 0;
	
	void createDescriptorHeaps();
	void createSwapChain();
	void initDevice();

	int m_Width = 0;
	int m_Height = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE getCurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE getDepthStencilView() const;

	void setViewport();
	bool createWindow();
	void createRenderTargets();
	void createStencilDepthBuffer();
	
public:
	window_dx12(int width, int height, const std::string& title, bool fullscreen);
	~window_dx12();
	void setSize(int width, int height) override;
	int getHeight() override;
	int getWidth() override;
	int getTargetFrameRate() override;
	void setTargetFrameRate(int fps) override;
	void setTitle(std::string title) override;
	void setVsync(bool enabled) override;
	std::string  getTitle() override;
	void draw() override;
};


#endif //WINDOW_DX12_H
