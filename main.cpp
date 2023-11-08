#include "src/window.h"
#include "src/shaderloader.h"
#include "src/shaderasset.h"

int main()
{
    /*
    using namespace modelViewer::res;
    shaderLoader loader;
    std::filesystem::path path("res/shaders/sample.glsl");
    auto shader = loader.load(path, shaderType::fragment);*/
    
    window window(500, 500, "Playground");
    return 0;
}