﻿#ifndef MODEL_VIEWER_MATERIAL_H
#define MODEL_VIEWER_MATERIAL_H
#include "../resource/model_info.h"
#include "light_directional.h"


namespace modelViewer::render
{
    class texture;
    class shader_program;
    class material
    {
    private:
        modelViewer::res::material_info m_Info;
        std::vector<std::shared_ptr<texture>> m_Textures;
        std::shared_ptr<shader_program> m_Program;
        
        const std::string  m_DiffuseSampler = "u_diffuseSampler";
        const std::string  m_NormalSampler = "u_normalSampler";
		const std::string  m_ShadowSampler = "u_shadowSampler";
        
        //TODO consider using a uniform block
        const std::string  m_MVPUniform = "m_MVP";
        const std::string  m_ModelViewUniform = "m_MV";
        const std::string  m_ModelUniform = "m_Model";
        const std::string  m_ProjectionUniform = "m_Projection";
		const std::string  m_LightViewProjectionUniform = "m_LightViewProjection";
        
        int m_MVPLocation = -1;
        int m_ModelViewLocation = -1;
        int m_ModelLocation = -1;
        int m_ProjectionLocation = -1;
		int m_LightViewProjectionLocation = -1;
		int m_ShadowMapSamplerLocation = -1;
        
        const std::string  m_AmbientAlbedo = "u_ambient";
        const std::string  m_DiffuseAlbedo = "u_diffuseAlbedo";
        const std::string  m_SpecularAlbedo = "u_specularAlbedo";
        const std::string  m_Shininess = "u_shininess";
        
        const std::string  m_LightPos = "u_light_pos";
        const std::string  m_LightColor = "u_light_color";

		const std::set<int> m_AssignedTextureLocations;
        
        void applyMaterialProperties();
        std::string  getSamplerName(modelViewer::res::texture_asset_type type);
    public:
        explicit material(const modelViewer::res::material_info& info, std::vector<std::shared_ptr<texture>>& textures, std::shared_ptr<shader_program>& program);
        void setMVP(glm::mat4& matrix);
        void setModelView(glm::mat4& matrix);
        void setProjection(glm::mat4& projection);
        void setModel(glm::mat4& model);
        void bind();
        int getUniformLocation(std::string name) const;
        int getAttributeLocation(std::string name) const;
        void setLight(const light_directional& light);
		void bindTextures(std::vector<std::shared_ptr<texture>>& textures);
		
		
		void setShadowMapSlot(int slot);
		void setLightViewProjection(glm::mat4& matrix);
    };
}


#endif 
