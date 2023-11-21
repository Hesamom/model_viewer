
#ifndef MODEL_VIEWER_MESH_LOADER_H
#define MODEL_VIEWER_MESH_LOADER_H



namespace modelViewer::res {
    class mesh_asset;
    class mesh_loader {
    public:
        std::shared_ptr<mesh_asset> load(std::filesystem::path filePath);
    };
}
#endif
