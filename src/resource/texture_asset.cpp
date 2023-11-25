﻿#include "texture_asset.h"

using namespace modelViewer::res;

texture_asset::texture_asset(modelViewer::res::byte* content, textureInfo info, std::string name) : m_Info{info}, m_Name{name} {
    std::unique_ptr<byte []> p(content);
    m_Content = std::move(p);
}

int texture_asset::getWidth() {
    return m_Info.width;
}

int texture_asset::getHeight() {
    return m_Info.height;
}

byte* texture_asset::getContent() {
    return m_Content.get();
}

int texture_asset::getChannelCount() {
    return m_Info.channels;
}

std::string &texture_asset::getName() {
    return m_Name;
}