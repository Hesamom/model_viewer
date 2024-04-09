
#include "window_win32.h"
#include <WindowsX.h>
#include "imgui/dx/imgui_impl_win32.h"
#include <shellscalingapi.h>

using namespace Microsoft::WRL;

window_win32* window_win32::current = nullptr;
bool window_win32::m_DPIAwarenessSet = false;

window_win32::window_win32(int width, int height, const std::string& title, bool fullscreen)
{
	if(current)
	{
		std::runtime_error("there can be only once active instance!");
	}

	current = this;
	
	//TODO add validations 
	m_RawClientWidth = width;
	m_RawClientHeight = height;
	m_Instance = GetModuleHandle(nullptr);

	auto result = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	if (result != S_OK) {
		throw std::runtime_error("failed to set DPI awareness");
	}
	m_DPIAwarenessSet = true;

	if (!createWindow())
	{
		throw std::runtime_error("failed to create window!" + std::to_string(GetLastError()));
	}
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return window_win32::current->MsgProc(hwnd, msg, wParam, lParam);
}

void window_win32::updateWindowForDpi() 
{ 
	int x, y;
	getPosition(x,y);
	int dpiScaledX = MulDiv(x, m_DPI, USER_DEFAULT_SCREEN_DPI); 
	int dpiScaledY = MulDiv(y, m_DPI, USER_DEFAULT_SCREEN_DPI); 
	int dpiScaledWidth = MulDiv(m_RawWindowWidth, m_DPI, USER_DEFAULT_SCREEN_DPI); 
	int dpiScaledHeight = MulDiv(m_RawWindowHeight, m_DPI, USER_DEFAULT_SCREEN_DPI);
	
	SetWindowPos(m_Handle, m_Handle, dpiScaledX, dpiScaledY, dpiScaledWidth, dpiScaledHeight, SWP_NOZORDER | SWP_NOACTIVATE); 
}

void window_win32::computeWindowSize()
{
	RECT R = { 0, 0, m_RawClientWidth, m_RawClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	m_RawWindowWidth  = R.right - R.left;
	m_RawWindowHeight = R.bottom - R.top;
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

	computeWindowSize();
	m_Handle = CreateWindow(L"MainWnd", mMainWndCaption.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, m_RawWindowWidth, m_RawWindowHeight, nullptr, nullptr, m_Instance, nullptr);
	
	if (!m_Handle)
	{
		MessageBox(nullptr, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	m_DPI = GetDpiForWindow(m_Handle);
	updateWindowForDpi();
	ShowWindow(m_Handle, SW_SHOW);
	UpdateWindow(m_Handle);

	return true;
}

void window_win32::setSize(int width, int height)
{
	if (width <= 0 || height <= 0) {
		throw std::invalid_argument("out of range values");
	}
	m_RawClientHeight = height;
	m_RawClientWidth = width;
	computeWindowSize();
	updateWindowForDpi();
}

int window_win32::getHeight()
{
	int scaled = MulDiv(m_RawClientHeight, m_DPI, USER_DEFAULT_SCREEN_DPI); 
	return scaled;
}

int window_win32::getWidth()
{
	int scaled = MulDiv(m_RawClientWidth, m_DPI, USER_DEFAULT_SCREEN_DPI); 
	return scaled;
}

void window_win32::getPosition(int& x, int& y) const {
	RECT windowRect;
	GetWindowRect(m_Handle, &windowRect);
	 x = windowRect.left;
	 y = windowRect.top;
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

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT window_win32::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return 0;
	
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
			m_RawClientWidth  = LOWORD(lParam);
			m_RawClientHeight = HIWORD(lParam);
			break;
		case WM_ENTERSIZEMOVE:
				m_Paused = true;
			break;
		case WM_EXITSIZEMOVE:
				m_Paused = false;
				m_SizeChangedCallback(m_RawClientWidth, m_RawClientHeight);
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


