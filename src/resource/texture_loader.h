
#ifndef MODEL_VIEWER_TEXTURE_LOADER_H
#define MODEL_VIEWER_TEXTURE_LOADER_H
#include "texture_asset.h"

namespace modelViewer::res {
    
    class texture_loader {
    private:
        std::unordered_map<std::filesystem::path, std::shared_ptr<texture_asset>> m_LoadedAssets;
    public:
        std::shared_ptr<texture_asset> load(const std::string& path, int channelsCount = 3, bool forceFlip = true);
    };

}

#endif 
