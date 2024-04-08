
#include "window_win32.h"
#include <WindowsX.h>
#include "imgui/dx/imgui_impl_win32.h"

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
	return m_ClientHeight;
}

int window_win32::getWidth()
{
	return m_ClientWidth;
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

void window_win32::onMouseCallback()
{
	window::mouse_event event = {m_MouseStates[0], m_MouseStates[1], m_MouseStates[2], m_MousePosX, m_MousePosY};
	m_MouseCallback(event);
}

void window_win32::onMouseStateChanged(int button, bool pressed)
{
	m_MouseStates[button] = pressed;
	onMouseCallback();
}

void window_win32::onMousePosChanged(int x, int y)
{
	m_MousePosX = x;
	m_MousePosY = y;

	onMouseCallback();
}

LRESULT window_win32::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		//return true;
	
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
			
		case WM_LBUTTONDOWN:
			onMouseStateChanged(0, true);
			return 0;
		case WM_MBUTTONDOWN:
			onMouseStateChanged(2, true);
			return 0;
		case WM_RBUTTONDOWN:
			onMouseStateChanged(1, true);
			return 0;
			
			
		case WM_LBUTTONUP:
			onMouseStateChanged(0, false);
			return 0;
		case WM_MBUTTONUP:
			onMouseStateChanged(2, false);
			return 0;
		case WM_RBUTTONUP:
			onMouseStateChanged(1, false);
			return 0;

		case WM_MOUSEWHEEL:
			onMouseScrollChanged(GET_WHEEL_DELTA_WPARAM(wParam));
			return 0;
			
		case WM_MOUSEMOVE:
			onMousePosChanged(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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
				m_SizeChangedCallback(m_ClientWidth, m_ClientHeight);
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

void* window_win32::getHandleRaw() {
	return m_Handle;
}

void window_win32::setOnMouseButtonCallback(std::function<void(mouse_event)> callback)
{
	m_MouseCallback = callback;
}

void window_win32::setOnMouseScrollCallback(std::function<void(int)> callback)
{
	m_MouseScrollCallback = callback;
}

void window_win32::onMouseScrollChanged(int offset)
{
	auto normalized = std::clamp(offset, -1, 1);
	m_MouseScrollCallback(normalized);
}

void window_win32::onNewImGUIFrame()
{
	ImGui_ImplWin32_NewFrame();
}

void window_win32::onInitImGUI()
{
	ImGui_ImplWin32_Init(m_Handle);
}

void window_win32::onShutdownImGUI()
{
	ImGui_ImplWin32_Shutdown();
}


