
#ifndef MODEL_VIEWER_MODELASSET_H
#define MODEL_VIEWER_MODELASSET_H

#include "mesh_asset.h"
#include "../common/transform.h"

namespace modelViewer::res {
    //TODO update to support a list of shaders 
    struct model_info {
        std::filesystem::path meshPath;
        std::filesystem::path texturePath;
        std::filesystem::path vertexShaderPath;
        std::filesystem::path fragmentShaderPath;
        modelViewer::common::transform transform;
        std::string  name;
    };
}

#endif
