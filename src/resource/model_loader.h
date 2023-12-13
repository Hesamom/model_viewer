
#ifndef MODEL_VIEWER_MODEL_LOADER_H
#define MODEL_VIEWER_MODEL_LOADER_H
#include "assimp/material.h"
#include "model_info.h"

namespace modelViewer::res {
    class mesh_asset;
    class model_info;
    class model_loader {
        
    private:
        const std::string  m_PhongVertPath = "res/shaders/sample/phong_phong_vert.glsl";
        const std::string  m_PhongFragPath = "res/shaders/sample/phong_phong_frag.glsl";

		const std::string  m_PhongNormalVertPath = "res/shaders/sample/phong_phong_normal_map_vert.glsl";
		const std::string  m_PhongNormalFragPath = "res/shaders/sample/phong_phong_normal_map_frag.glsl";
		
        const std::string  m_GouraudVertPath = "res/shaders/sample/phong_gouraud_vert.glsl";
        const std::string  m_GouraudFragPath = "res/shaders/sample/phong_gouraud_frag.glsl";

        void setShaders(aiMaterial& material, std::vector<shader_asset_info>& shaders);
        void setMaterialProperties(aiMaterial& material, material_property_set& set);
    public:
        void load(std::string filePath, model_info& info);
        std::shared_ptr<mesh_asset> loadMesh(std::string filePath);
    };
}
#endif
