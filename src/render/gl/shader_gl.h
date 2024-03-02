
#ifndef MODEL_VIEWER_SHADER_H
#define MODEL_VIEWER_SHADER_H

#include "../../resource/shader_asset.h"

namespace modelViewer::render {

    class shader_gl {
    public:
        explicit shader_gl(std::shared_ptr<res::shader_asset>& asset);
        
        void compile();
        void destroy() const;
        std::string  getCompilationLog();
        bool isCompiled();
		void verify();
        res::shaderType getType();
        unsigned int getId() const;

    private:
        unsigned int m_ShaderId = 0;
        std::shared_ptr<res::shader_asset> m_Asset;
    };
}
#endif //MODEL_VIEWER_SHADER_H
