
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
        
        void compile();
        void destroy() const;
        std::string  getCompilationLog();
        bool isCompiled();
		void verify();
        res::shaderType getType();
        unsigned int getId() const;
    };
}
#endif //MODEL_VIEWER_SHADER_H
