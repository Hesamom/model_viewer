
#ifndef RENDERDOC_H
#define RENDERDOC_H

#include "renderdoc_app.h"
#include "tracer.h"

namespace modelViewer::debug {
	class renderdoc : public tracer {
	private:
		RENDERDOC_API_1_1_2 *m_Api = nullptr;
		void* m_Window = nullptr;
		void* m_Context = nullptr;
		
		const std::string m_RenderdocPath = "\"C:\\Program Files\\RenderDoc\\renderdoccmd.exe\"";
		
	public:
		renderdoc();
		void startCapture() override;
		void endCapture() override;

		void setContext(void* context);
		void setWindow(void* window);
		void showUI() override;
	};
}

#endif //RENDERDOC_H
