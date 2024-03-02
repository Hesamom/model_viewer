
#ifndef WINDOW_WIN32_H
#define WINDOW_WIN32_H


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

class window_win32 : public window{
	
private:
	
	std::wstring mMainWndCaption = L"d3d App";
	HWND  m_Handle = nullptr;
	HINSTANCE m_Instance = nullptr;
	std::function<void(int, int)> m_SizeChangedCallback;


	MSG m_LastWindowMessage {0};

	int m_Width = 0;
	int m_Height = 0;
	
	bool createWindow();
	
public:
	window_win32(int width, int height, const std::string& title, bool fullscreen);
	~window_win32();
	void setSize(int width, int height) override;
	int getHeight() override;
	int getWidth() override;
	void setTitle(std::string title) override;
	std::string  getTitle() override;

	void setOnSizeChangedCallback(std::function<void(int, int)> callback) override;
	HWND  getHandle();
	bool shouldClose() override;
	bool isPaused() override;
	void pollEvents() override;

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static window_win32* current;
	bool mAppPaused = false;
};


#endif //WINDOW_WIN32_H
