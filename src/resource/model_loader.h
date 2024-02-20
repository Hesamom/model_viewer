
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
		
    public:
        void load(std::string filePath, model_info& info);
        std::shared_ptr<mesh_asset> loadFirstMesh(std::string filePath);
		model_loader();
		~model_loader();
    };
}
#endif
