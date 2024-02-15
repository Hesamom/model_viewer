#include "src/resource/shader_asset.h"
#include "src/window/modelviewer_window.h"

int main()
{
    using namespace modelViewer::res;
    modelviewer_window window(1024, 1024, "Playground", false);
    window.setClearFlag(glm::vec4(0.15f,0.15f,0.15f,0));
	
    window.draw();
    return 0;
}