#ifndef MODEL_VIEWER_SHADERASSET_H
#define MODEL_VIEWER_SHADERASSET_H

namespace modelViewer::res {

    enum class shaderType
    {
        none,
        fragment,
        vertex
    };

    class shaderAsset{
    private:
        std::string m_Source;
        shaderType m_Type;
    public:
        shaderAsset(const std::string& source, shaderType type);
        std::string getSource();
        shaderType getType();
    };
}

#endif //MODEL_VIEWER_SHADERASSET_H
