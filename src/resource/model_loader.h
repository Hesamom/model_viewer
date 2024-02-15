
#ifndef MODEL_VIEWER_MODEL_LOADER_H
#define MODEL_VIEWER_MODEL_LOADER_H
#include "assimp/material.h"
#include "model_info.h"
#include "assimp/scene.h"

namespace modelViewer::res {
    class mesh_asset;
    class model_info;
	
	
    class model_loader {
        
    private:
		
		std::shared_ptr<material_asset> m_DefaultMaterialAsset;
		
		void ProcessNodeMaterial(aiNode& node,
			const aiScene& scene,
			model_info& modelInfo,
			std::vector<std::shared_ptr<material_asset>>& materials);
		std::shared_ptr<material_asset> getMaterial(const aiMesh& mesh, const aiScene& scene,  model_info& modelInfo);
        void setShaders(aiMaterial& material, std::vector<shader_asset_info>& shaders);
        void setMaterialProperties(aiMaterial& material, material_property_set& set);
    public:
        void load(std::string filePath, model_info& info);
        std::shared_ptr<mesh_asset> loadFirstMesh(std::string filePath);
		model_loader();
		~model_loader();
    };
}
#endif
