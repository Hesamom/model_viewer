
#include "texture_loader.h"
#include <ReadImage.h>
#include "texture_asset.h"

using namespace modelViewer::res;

byte * readFile(std::string &path, textureInfo* info) {
    
    //TODO add a flag later?
    stbi_set_flip_vertically_on_load(1);
    
    byte* content = stbi_load(path.c_str(), &(info->width), &(info->height), &(info->channels), info->channels);
    return content;
}


std::shared_ptr<texture_asset> modelViewer::res::texture_loader::load(std::string &path, int channelsCount) {

    if (m_LoadedAssets.contains(path))
    {
        return m_LoadedAssets[path];
    }

    if (path.empty())
    {
        throw std::runtime_error("can not load empty path!");
    }

    if (!std::filesystem::exists(path))
    {
        throw std::runtime_error("file at given path does not exits!, path:" + path);
    }

    textureInfo info;
    info.channels = channelsCount;
    auto content = readFile(path, &info);
    auto asset = std::make_shared<texture_asset>(content, info, path);
    m_LoadedAssets[path] = asset;

    assert(m_LoadedAssets.contains(path));
    return asset;
}

