#include "src/window/modelviewer_app.h"
#include "src/window/window_win32.h"
#include "src/render/gfx_device_dx.h"
#include "src/render/gfx_device_gl.h"

void getWindow(bool dx);
bool isUsingDX(int count, const char** cmd);

std::shared_ptr<window> window;
std::shared_ptr<gfx_device> device;

int main(int count, const char** cmd)
{
	getWindow(isUsingDX(count, cmd));
	modelviewer_app app(window, device);
	
	app.loop();
	
    return 0;
}

void getWindow(bool dx)
{
	if(dx)
	{
		auto win32 = std::make_shared<window_win32>(1024,1024, "dx12", false);
		auto dx = std::make_shared<modelViewer::render::gfx_device_dx>(win32);
		device = dx;
		window = win32;
		return;
	}

	auto winGL =  std::make_shared<window_glfw>(1024, 1024, "Playground", false);
	device = std::make_shared<gfx_device_gl>(winGL);
	window = winGL;
}


bool isUsingDX(int count, const char** cmd)
{
	if(count > 1)
	{
		std::string firstCmd = cmd[1];
		if (firstCmd == "-dx")
		{
			return true;
		}
	}

	return false;
}

/*int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	
}*/