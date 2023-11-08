#include "shaderloader.h"
#include "shaderasset.h"

using namespace modelViewer::res;

std::shared_ptr<shaderAsset> modelViewer::res::shaderLoader::load(std::filesystem::path &path, shaderType type)
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
    auto asset = std::make_shared<shaderAsset>(source, type);
    m_LoadedAssets[path] = asset;
    
    assert(m_LoadedAssets.contains(path));
    return asset;
}

std::string shaderLoader::readFile(std::filesystem::path &path) {

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
