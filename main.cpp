#define GLM_FORCE_LEFT_HANDED

#include "src/window/modelviewer_app.h"
#include "src/window/window_win32.h"
#include "src/render/dx/gfx_device_dx.h"
#include "src/render/gl/gfx_device_gl.h"

void getWindow(bool dx);
bool isUsingDX(int count, const char** cmd);
bool attachRenderdoc(int count, const char** cmd);
bool containsArg(int count, const char** cmd, const char* arg);

std::shared_ptr<window> window;
std::shared_ptr<modelViewer::render::gfx_device> device;


int main(int count, const char** cmd)
{
	std::shared_ptr<modelViewer::debug::renderdoc> renderDoc;
	if (attachRenderdoc(count, cmd))
	{
		auto renderdoc = std::make_shared<modelViewer::debug::renderdoc>();
		renderdoc->showUI();
		renderDoc = renderdoc;
	}

	getWindow(isUsingDX(count, cmd));
	
	if(renderDoc)
	{
		renderDoc->setContext(device->getDevice());
		renderDoc->setWindow(window->getHandleRaw());
	}

	std::shared_ptr<modelViewer::debug::tracer> tracer = renderDoc;
	modelviewer_app app(window, device, tracer, true);
	
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
	device = std::make_shared<modelViewer::render::gfx_device_gl>(winGL);
	window = winGL;
}

bool attachRenderdoc(int count, const char** cmd)
{
	return containsArg(count, cmd, "-doc");
}


bool isUsingDX(int count, const char** cmd)
{
	return containsArg(count, cmd, "-dx");
}

bool containsArg(int count, const char** cmds, const char* arg)
{
	for(int i = 1; i < count; i++)
	{
		if(strcmp(cmds[i], arg) == 0)
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