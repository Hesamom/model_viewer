#ifndef MODEL_VIEWER_SHADER_ASSET_H
#define MODEL_VIEWER_SHADER_ASSET_H

namespace modelViewer::res {

    enum class shaderType
    {
        none,
        fragment,
        vertex
    };

    class shader_asset{
    private:
        std::string m_Source;
        shaderType m_Type;
    public:
        shader_asset(const std::string& source, shaderType type);
        std::string getSource();
        shaderType getType();
    };
}

#endif //MODEL_VIEWER_SHADER_ASSET_H
