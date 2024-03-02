
#include "window_win32.h"

using namespace Microsoft::WRL;

window_win32* window_win32::current = nullptr;

window_win32::window_win32(int width, int height, const std::string& title, bool fullscreen)
{
	if(window_win32::current)
	{
		std::runtime_error("there can be only once active instance!");
	}

	window_win32::current = this;
	
	//TODO add validations 
	m_ClientWidth = width;
	m_ClientHeight = height;
	m_Instance = GetModuleHandle(nullptr);

	if (!createWindow())
	{
		throw std::runtime_error("failed to create window!" + std::to_string(GetLastError()));
	}
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return window_win32::current->MsgProc(hwnd, msg, wParam, lParam);
}

bool window_win32::createWindow()
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
	RECT R = { 0, 0, m_ClientWidth, m_ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	m_Handle = CreateWindow(L"MainWnd", mMainWndCaption.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, m_Instance, nullptr);
	if (!m_Handle)
	{
		MessageBox(nullptr, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_Handle, SW_SHOW);
	UpdateWindow(m_Handle);

	return true;
}

void window_win32::setSize(int width, int height)
{

}

int window_win32::getHeight()
{
	return 0;
}

int window_win32::getWidth()
{
	return 0;
}


void window_win32::setTitle(std::string title)
{

}


std::string window_win32::getTitle()
{
	return std::string();
}

window_win32::~window_win32()
{

}

bool window_win32::shouldClose()
{
	return m_LastWindowMessage.message == WM_QUIT;
}

bool window_win32::isPaused()
{
	return m_Paused;
}

void window_win32::pollEvents()
{
	if(PeekMessage( &m_LastWindowMessage, 0, 0, 0, PM_REMOVE ))
	{
		TranslateMessage( &m_LastWindowMessage );
		DispatchMessage( &m_LastWindowMessage );
	}
}

LRESULT window_win32::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE )
			{
				m_Paused = true;
			}
			else
			{
				m_Paused = false;
			}
			return 0;
		case WM_SIZE:
			m_ClientWidth  = LOWORD(lParam);
			m_ClientHeight = HIWORD(lParam);
			break;
		case WM_ENTERSIZEMOVE:
				m_Paused = true;
			break;
		case WM_EXITSIZEMOVE:
				m_Paused = false;
				m_SizeChangedCallback(m_ClientWidth, m_ClientWidth);
		break;
				
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

HWND window_win32::getHandle()
{
	return m_Handle;
}

void window_win32::setOnSizeChangedCallback(std::function<void(int, int)> callback)
{
	m_SizeChangedCallback = callback;
}
