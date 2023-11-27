#include "shader_loader.h"
#include "shader_asset.h"

using namespace modelViewer::res;

std::string readFile(const std::filesystem::path &path) {

    std::ifstream stream(path);

    if (!stream.is_open())
    {
        throw std::runtime_error("file with path:" + path.string() + " can not be opened!");
    }

    std::string content;
    std::string line;
    while (getline(stream, line))
    {
        content += line;
        content += "\n";
    }

    return content;
}


std::shared_ptr<shader_asset> modelViewer::res::shader_loader::load(const std::filesystem::path &path, shaderType type)
{
    if (m_LoadedAssets.contains(path))
    {
        return m_LoadedAssets[path];
    }

    if (path.empty())
    {
        throw std::runtime_error("can not load empty path!");
    }

    using namespace std::string_literals;
    if (!std::filesystem::exists(path))
    {
        throw std::runtime_error("file at given path does not exits!, path:" + path.string());
    }
    
    auto source = readFile(path);
    auto asset = std::make_shared<shader_asset>(source, type, path.string());
    m_LoadedAssets[path] = asset;
    
    assert(m_LoadedAssets.contains(path));
    return asset;
}

