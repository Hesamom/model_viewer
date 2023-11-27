#ifndef MODEL_VIEWER_SHADER_LOADER_H
#define MODEL_VIEWER_SHADER_LOADER_H


namespace modelViewer::res {

    class shader_asset;
    enum class shaderType;
    
    class shader_loader {
    private:
        std::unordered_map<std::filesystem::path, std::shared_ptr<shader_asset>> m_LoadedAssets;
    public:
        std::shared_ptr<shader_asset> load(const std::filesystem::path& filePath, shaderType type);
    };

}

#endif //MODEL_VIEWER_SHADER_LOADER_H
