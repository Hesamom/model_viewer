
#ifndef MODEL_VIEWER_MODELASSET_H
#define MODEL_VIEWER_MODELASSET_H

#include "mesh_asset.h"
#include "../common/transform.h"
#include "shader_loader.h"

using color = glm::vec3;

namespace modelViewer::res
{
    enum class texture_asset_type
    {
        none,
        diffuse,
        normal,
        cube
    };
    
    enum class texture_asset_mapping_mode
    {
        unsupported,
        uv
    };
    
    enum class texture_asset_wrapping_mode
    {
        unsupported,
        wrap,
        clamp,
        mirror
    };

    struct texture_asset_info
    {
        std::vector<std::string> paths;
        unsigned int uvIndex = 0;
        texture_asset_mapping_mode mappingMode = texture_asset_mapping_mode::uv;
        texture_asset_wrapping_mode wrappingMode = texture_asset_wrapping_mode::wrap;
        texture_asset_type type = texture_asset_type::diffuse;
    };
    
    struct shader_asset_info
    {
        std::string path;
        shaderType type;
    };
    
    struct material_property_set
    {
       color diffuseAlbedo = color(0.2f);
       color ambient =  color(0.5f);
       color specularAlbedo =  color(0.5f);
       float shininess = 128;
    };
    
    struct material_info
    {
        std::vector<texture_asset_info> textures;
        std::vector<shader_asset_info> shaders;
        material_property_set propertySet;
    };
    
    struct model_info {
        std::shared_ptr<mesh_asset> mesh;
        material_info material;
        modelViewer::common::transform transform;
        std::string name;
        std::string path;
    };
}

#endif
