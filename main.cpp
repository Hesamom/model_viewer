﻿#include "src/window/modelviewer_app.h"

#ifdef GFX_DX
	#include "src/window/dx/window_win32.h"
	#include "src/render/dx/gfx_device_dx.h"
#else
	#include "src/render/gl/gfx_device_gl.h"
#endif

void initWindow();
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

	initWindow();
	
	if(renderDoc)
	{
		renderDoc->setContext(device->getDevice());
		renderDoc->setWindow(window->getHandleRaw());
	}

	std::shared_ptr<modelViewer::debug::tracer> tracer = renderDoc;
	modelviewer_app app(window, device, tracer, false);
	
	app.loop();
	
    return 0;
}

void initWindow()
{
#ifdef GFX_DX
	{
		auto win32 = std::make_shared<window_win32>(720,720, "dx12", false);
		auto dx = std::make_shared<modelViewer::render::dx::gfx_device_dx>(win32);
		device = dx;
		window = win32;
	}
#else
	auto winGL =  std::make_shared<window_glfw>(720, 720, "gl", false);
	device = std::make_shared<modelViewer::render::gfx_device_gl>(winGL);
	window = winGL;
#endif
}

bool attachRenderdoc(int count, const char** cmd)
{
	return containsArg(count, cmd, "-doc");
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