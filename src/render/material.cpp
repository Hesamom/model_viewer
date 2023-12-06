﻿#include "material.h"
#include "texture.h"
#include "shader_program.h"

using namespace modelViewer::render;
using namespace modelViewer::res;


material::material(material_info &info, std::vector<std::shared_ptr<texture>>& textures, std::shared_ptr<shader_program>& program) {

    m_Info = info;
    m_Program = program;

    m_Program->bind();
    m_ModelLocation = m_Program->getUniformLocation(m_ModelViewUniform);;
    m_ModelViewLocation = m_Program->getUniformLocation(m_ModelViewUniform);
    m_MVPLocation = m_Program->getUniformLocation(m_MVPUniform);
    m_ProjectionLocation = m_Program->getUniformLocation(m_ProjectionUniform);
    
    applyMaterialProperties();
    bindTextures(textures);
}

std::string material::getSamplerName(modelViewer::res::texture_asset_type type) {
    switch (type) {

        case texture_asset_type::none:
            break;
        case texture_asset_type::diffuse:
            return m_DiffuseSampler;
        case texture_asset_type::normal:
            return m_NormalSampler;
        default:
            throw std::runtime_error("not supported!");
    }
    return {};
}

void material::setMVP(glm::mat4 &matrix) 
{
    if (m_MVPLocation < 0)
    {
        return;
    }
    
    m_Program->setUniformMatrix4(m_MVPLocation, matrix);
}

void material::setModelView(glm::mat4 &matrix) {
    if (m_ModelViewLocation < 0)
    {
        return;
    }

    m_Program->setUniformMatrix4(m_ModelViewLocation, matrix);
}

void
material::setModel(glm::mat4 &model) {
    if (m_ModelLocation < 0)
    {
        return;
    }

    m_Program->setUniformMatrix4(m_ModelLocation, model);
}

void material::setProjection(glm::mat4 &projection) {
    if (m_ProjectionLocation < 0)
    {
        return;
    }

    m_Program->setUniformMatrix4(m_ProjectionLocation, projection);
}

void material::applyMaterialProperties() {
    
    int ambientLoc = m_Program->getUniformLocation(m_AmbientAlbedo);
    if (ambientLoc > -1)
    {
        m_Program->setUniformVector3(ambientLoc, m_Info.propertySet.ambient);
    }

    int diffuseLoc = m_Program->getUniformLocation(m_DiffuseAlbedo);
    if (diffuseLoc > -1)
    {
        m_Program->setUniformVector3(diffuseLoc, m_Info.propertySet.diffuseAlbedo);
    }

    int specLoc = m_Program->getUniformLocation(m_SpecularAlbedo);
    if (specLoc > -1)
    {
        m_Program->setUniformVector3(specLoc, m_Info.propertySet.specularAlbedo);
    }

    int shinLoc = m_Program->getUniformLocation(m_Shininess);
    if (shinLoc > -1)
    {
        m_Program->setUniformFloat(shinLoc, m_Info.propertySet.shininess);
    }
}

void material::bindTextures(std::vector<std::shared_ptr<texture>>& textures) 
{
    int index = 0;
    for (const auto &item: textures)
    {
        auto loc = m_Program->getUniformLocation(getSamplerName(item->getType()));
        if (loc < 0)
        {
            continue;
        }
        
        item->active(index);
        m_Program->bind();
        m_Program->setUniformInt(loc, index);
        index++;
        m_Textures.push_back(item);
    }
}

void material::bind() {
    m_Program->bind();
    
    int index = 0;
    for (const auto &item: m_Textures)
    {
        item->active(index);
        index++;
    }
}

int material::getUniformLocation(std::string name) {
    return m_Program->getUniformLocation(name);
}

int material::getAttributeLocation(std::string name) {
    return m_Program->getAttributeLocation(name);
}

void material::setLight(const light_directional &light) {
    
    m_Program->bind();
    int lightPos = m_Program->getUniformLocation(m_LightPos);
    if (lightPos > -1)
    {
        m_Program->setUniformVector3(lightPos, light.getPosition());
    }

    int lightColor = m_Program->getUniformLocation(m_LightColor);
    if (lightColor > -1)
    {
        m_Program->setUniformVector3(lightColor, light.getColor());
    }
}