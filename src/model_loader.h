
#ifndef MODEL_VIEWER_MODEL_LOADER_H
#define MODEL_VIEWER_MODEL_LOADER_H



namespace modelViewer::res {
    class mesh_asset;
    class model_loader {
    public:
        mesh_asset load(std::filesystem::path filePath);
    };
}
#endif
