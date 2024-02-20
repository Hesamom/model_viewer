
#ifndef RENDERDOC_H
#define RENDERDOC_H

#include "renderdoc_app.h"

class renderdoc {
private:
	RENDERDOC_API_1_1_2 *rdoc_api = nullptr;
public:
	void init();
	void startCapture();
	void EndCapture();
};


#endif //RENDERDOC_H
