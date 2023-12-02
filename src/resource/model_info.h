
#ifndef MODEL_VIEWER_MODELASSET_H
#define MODEL_VIEWER_MODELASSET_H

#include "mesh_asset.h"
#include "../common/transform.h"
#include "material.h"

namespace modelViewer::res {
    //TODO update to support a list of shaders 
    struct model_info {
        std::string meshPath;
        std::string texturePath;
        std::string vertexShaderPath;
        std::string fragmentShaderPath;
        modelViewer::common::transform transform;
        material material;
        std::string  name;
        
    };
}

#endif
