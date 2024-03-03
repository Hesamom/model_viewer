#include <windows.h>
#include "renderdoc.h"

using namespace modelViewer::debug;

void renderdoc::startCapture()
{
	if (m_Api)
	{
		m_Api->StartFrameCapture(m_Context, m_Window);
		std::cout << "started capure frame \n";
	}
}

void renderdoc::endCapture()
{
	if (m_Api)
	{
		m_Api->EndFrameCapture(m_Context, m_Window);
		std::cout << "ended capture frame \n";
	}
}

void renderdoc::setContext(void* context) {
	m_Context = context;
}

void renderdoc::setWindow(void* window) {
	m_Window = window;
}

void createProcess2(const std::string& imagePath, std::string& cmd)
{
	int result = system((imagePath + cmd).c_str());
	std::cout<< "process launched with result: " << result << "\n";
}

void createProcess1(LPCSTR lpApplicationName, LPSTR cmd)
{
	// additional information
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	auto result = CreateProcessA
		(
			lpApplicationName,   // the path
			cmd,                // Command line
			NULL,                   // Process handle not inheritable
			NULL,                   // Thread handle not inheritable
			FALSE,                  // Set handle inheritance to FALSE
			CREATE_NEW_CONSOLE,     // Opens file in a separate console
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi           // Pointer to PROCESS_INFORMATION structure
		);
	
	if(result)
	{
		std::cout<< "Created Process \n";
	}
	else
	{
		std::cout<< "Create Process Error: " << GetLastError() << "\n";
	}

}

renderdoc::renderdoc() {

	unsigned int processId = GetCurrentProcessId();
	assert(processId > 0);
	
	std::string cmd = " inject --PID " + std::to_string(processId);
	createProcess2(m_RenderdocPath, cmd);
	
	if(HMODULE mod = GetModuleHandleA("renderdoc.dll"))
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI =(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **)&m_Api);
		assert(ret == 1);
		
		if (ret == 1)
		{
			auto path = m_Api->GetCaptureFilePathTemplate();
			std::cout << "initiated renderdoc successfully!, path:" << path << std::endl;
		}
	}
	else
	{
		std::cerr << "failed to initiate renderdoc!\n";
	}
}

void renderdoc::showUI()
{
	if (m_Api)
	{
		m_Api->LaunchReplayUI(1, nullptr);
	}
}
