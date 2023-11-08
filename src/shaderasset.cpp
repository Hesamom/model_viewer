#include "shaderasset.h"

using namespace  modelViewer::res;

shaderAsset::shaderAsset(const std::string& source, modelViewer::res::shaderType type) : m_Type{type} {
    
    if (source.empty())
    {
        throw std::runtime_error("source must be non-empty!");
    }
    
    m_Source = source;
}

std::string shaderAsset::getSource() {
    return m_Source;
}

shaderType shaderAsset::getType() {
    return m_Type;
}
