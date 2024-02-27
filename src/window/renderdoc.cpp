#include <windows.h>
#include "renderdoc.h"


void renderdoc::init(){
	if(HMODULE mod = LoadLibrary("res/libs/renderdoc.dll")){
		pRENDERDOC_GetAPI RENDERDOC_GetAPI =
			(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **)&rdoc_api);
		assert(ret == 1);

		if (ret == 1){
			std::cout << "initiated renderdoc successfully!\n";
		}
	}
	else{
		std::cerr << "failed to initiate renderdoc!\n";
		std::cerr << GetLastError() <<"\n";
	}
}

void renderdoc::startCapture(){
	if (rdoc_api){
		std::cout << "Renderdoc: Start capture\n";
		rdoc_api->LaunchReplayUI(1, nullptr);
		rdoc_api->SetCaptureOptionF32(eRENDERDOC_Option_DelayForDebugger, 1.0f);
		rdoc_api->StartFrameCapture(NULL, NULL);
		rdoc_api->SetCaptureTitle("test");
	}
}

void renderdoc::EndCapture(){
	if (rdoc_api){
		std::cout << "Renderdoc: End capture\n";
		rdoc_api->EndFrameCapture(NULL, NULL);
		rdoc_api->ShowReplayUI();
	}
}
