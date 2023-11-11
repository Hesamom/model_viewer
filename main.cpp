﻿#include "src/window.h"
#include "src/shader_loader.h"
#include "src/shader_asset.h"
#include "src/texture_loader.h"
#include "src/model_loader.h"
#include "src/mesh_asset.h"

int main()
{
    //an example of loading a shader 
    //'res' folder in the source directory is copied to bin folder in each build to be accessible 
    using namespace modelViewer::res;
    shader_loader sLoader;
    std::filesystem::path sPath("res/shaders/sample/sample_vert.glsl");
    auto shader = sLoader.load(sPath, shaderType::fragment);

    //an example of loading a texture
    texture_loader tLoader;
    std::filesystem::path tPath("res/textures/sample.png");
    auto texture = tLoader.load(tPath);

    //an example of loading a model
    model_loader mLoader;
    std::filesystem::path mPath("res/models/primitives/sphere.fbx");
    mesh_asset mesh = mLoader.load(mPath);
    
    window window(500, 500, "Playground");
    return 0;
}