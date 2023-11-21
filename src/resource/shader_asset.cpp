#include "shader_asset.h"

using namespace  modelViewer::res;

shader_asset::shader_asset(const std::string& source, modelViewer::res::shaderType type,  const std::string& path)
: m_Type{type}, m_Path{path} {
    
    if (source.empty())
    {
        throw std::runtime_error("source must be non-empty!");
    }
    
    m_Source = source;
}


std::string shader_asset::getSource() {
    return m_Source;
}

shaderType shader_asset::getType() {
    return m_Type;
}

const std::string &shader_asset::getPath() const {
    return m_Path;
}
