﻿
#ifndef WINDOW_WIN32_H
#define WINDOW_WIN32_H

#include <windows.h>
#include <wrl.h>
#include "../window.h"

class window_win32 : public window{
	
private:
	
	std::wstring mMainWndCaption = L"d3d App";
	HWND  m_Handle = nullptr;
	HINSTANCE m_Instance = nullptr;
	std::function<void(int, int)> m_SizeChangedCallback;
	std::function<void (mouse_event)> m_MouseCallback;
	std::function<void(int)> m_MouseScrollCallback;
	
	MSG m_LastWindowMessage {0};

	int m_RawClientWidth = 0;
	int m_RawClientHeight = 0;
	
	LONG m_RawWindowWidth = 0;
	LONG m_RawWindowHeight = 0;
	
	UINT m_DPI = USER_DEFAULT_SCREEN_DPI;
	
	int m_MousePosX = 0;
	int m_MousePosY = 0;
	
	bool m_Paused = false;
	bool m_MouseStates[3]{};
	
	static bool m_DPIAwarenessSet;

	void onMouseScrollChanged(int offset);
	void onMouseCallback();
	void onMouseStateChanged(int button, bool pressed);
	void onMousePosChanged(int x, int y);

	void updateWindowForDpi();

	void computeWindowSize();

	bool createWindow();
	
public:
	window_win32(int width, int height, const std::string& title, bool fullscreen);
	~window_win32();
	void setSize(int width, int height) override;
	int getHeight() override;
	int getWidth() override;
	void getPosition(int& x, int& y) const;
	void setTitle(std::string title) override;
	std::string  getTitle() override;

	void setOnSizeChangedCallback(std::function<void(int, int)> callback) override;
	void setOnMouseButtonCallback(std::function<void (mouse_event)> callback) override;
	void setOnMouseScrollCallback(std::function<void(int)> callback) override;
	
	void onNewImGUIFrame() override;
	void onInitImGUI();
	void onShutdownImGUI() override;
	
	void* getHandleRaw() override;
	HWND  getHandle();
	bool shouldClose() override;
	bool isPaused() override;
	void pollEvents() override;

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static window_win32* current;
};


#endif //WINDOW_WIN32_H