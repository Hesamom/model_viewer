#ifndef MODEL_VIEWER_SHADER_ASSET_H
#define MODEL_VIEWER_SHADER_ASSET_H

namespace modelViewer::res {

    enum class shaderType
    {
        none,
		vertex,
        tesselation_control,
        tesselation_evaluation,
        geometry,
		fragment,
        compute
    };

    class shader_asset{
    private:
        std::string m_Source;
        shaderType m_Type;
        const std::string m_Path;
    public:
        shader_asset(const std::string& source, shaderType type, const std::string& path);
        std::string getSource();
        shaderType getType();
        const std::string& getPath() const;
    };
}

#endif //MODEL_VIEWER_SHADER_ASSET_H
