#ifndef MODEL_VIEWER_SHADERLOADER_H
#define MODEL_VIEWER_SHADERLOADER_H


namespace modelViewer::res {

    class shaderAsset;
    enum class shaderType;
    
    class shaderLoader {
    private:
        std::unordered_map<std::filesystem::path, std::shared_ptr<shaderAsset>> m_LoadedAssets;
        std::string readFile(std::filesystem::path& path);
    public:
        std::shared_ptr<shaderAsset> load(std::filesystem::path& path, shaderType type);
    };

}

#endif //MODEL_VIEWER_SHADERLOADER_H
