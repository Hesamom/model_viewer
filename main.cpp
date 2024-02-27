#include "src/window/modelviewer_window.h"
#include "src/window/window_dx12.h"


window* getWindow(bool dx)
{
	if(dx)
	{
		auto* win = new window_dx12(1024,1024, "dx12", false); 
		return win;
	}

	auto* gl =  new modelviewer_window(1024, 1024, "Playground", false);
	return gl;
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

int main(int count, const char** cmd)
{
	auto window = getWindow(isUsingDX(count, cmd));
	window->draw();
    return 0;
}

/*int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	
}*/