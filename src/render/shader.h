
#ifndef MODEL_VIEWER_SHADER_H
#define MODEL_VIEWER_SHADER_H

#include "../resource/shader_asset.h"

namespace modelViewer::render {

    class shader {
    private:
        unsigned int m_ShaderId = 0;
        std::shared_ptr<modelViewer::res::shader_asset> m_Asset;
    public:
        explicit shader(std::shared_ptr<modelViewer::res::shader_asset>& asset);
        ~shader();
        
        void compile();
        std::string  getCompilationLog();
        bool isCompiled();
        res::shaderType getType();
        unsigned int getId() const;
    };
}
#endif //MODEL_VIEWER_SHADER_H
