﻿#include "texture_loader.h"
#include <ReadImage.h>
#include "texture_asset.h"

using namespace modelViewer::res;

byte * readFile(const std::string &path, textureInfo* info) {
    
    stbi_set_flip_vertically_on_load(info->forceFlip);
    
    byte* content = stbi_load(path.c_str(), &(info->width), &(info->height), &(info->channels), info->channels);
    return content;
}


std::shared_ptr<texture_asset> texture_loader::load(const std::string &path, int channelsCount, bool forceFlip) {

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
    info.forceFlip = forceFlip;
    info.channels = channelsCount;
    auto content = readFile(path, &info);
    auto asset = std::make_shared<texture_asset>(content, info, path);
    m_LoadedAssets[path] = asset;

    assert(m_LoadedAssets.contains(path));
    return asset;
}

