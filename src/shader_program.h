#ifndef MODEL_VIEWER_SHADER_PROGRAM_H
#define MODEL_VIEWER_SHADER_PROGRAM_H

#include "shader_loader.h"

namespace modelViewer::render{
    class shader_program {
        class shader_asset;
private:
        std::shared_ptr<modelViewer::res::shader_asset> m_VertexShaderAsset;
        std::shared_ptr<modelViewer::res::shader_asset> m_FragmentShaderAsset;
    public:
        //TODO: only needs shader asset
        //TODO:
        shader_program(modelViewer::res::shader_loader shaderLoader, std::filesystem::path& shaderVertexFilePath, std::filesystem::path& shaderFragmentFilePath);
};
}


#endif
