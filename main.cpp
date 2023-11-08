#include "src/window.h"
#include "src/shaderloader.h"
#include "src/shaderasset.h"

int main()
{
    //an example of loading a shader 
    //'res' folder in the source directory is copied to bin folder in each build to be accessible 
    using namespace modelViewer::res;
    shaderLoader loader;
    std::filesystem::path path("res/shaders/sample.glsl");
    auto shader = loader.load(path, shaderType::fragment);
    
    
    window window(500, 500, "Playground");
    return 0;
}