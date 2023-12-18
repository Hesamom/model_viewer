﻿#ifndef MODEL_VIEWER_MATERIAL_H
#define MODEL_VIEWER_MATERIAL_H
#include "../resource/model_info.h"
#include "light_directional.h"
#include "texture.h"
#include "shader_uniform.h"
#include "light_point.h"
#include "shader_program.h"


namespace modelViewer::render
{
    class texture_2D;
    class shader_program;
    class material
    {
    private:
        res::material_info m_Info;
        std::vector<std::shared_ptr<texture>> m_ActiveTextures;
        std::shared_ptr<shader_program> m_Program;
    	
        //TODO consider using a uniform block
    	const std::string  m_ShadowSampler = "u_shadowSampler";
		shader_uniform<glm::mat4> m_MVPUniform{"m_MVP", ""};
		shader_uniform<glm::mat4> m_ModelViewUniform{"m_MV", ""};
		shader_uniform<glm::mat4> m_ModelUniform{"m_Model", ""};
		shader_uniform<glm::mat4> m_ProjectionUniform{"m_Projection",""};
		
		shader_uniform<glm::mat4> m_LightViewProjectionUniform{"m_LightViewProjection",""};
		shader_uniform<glm::vec3> m_LightPosUniform{"u_light_pos",""};
		shader_uniform<glm::vec3> m_LightAmbientUniform{"u_lightAmbient",""};
		shader_uniform<glm::vec3> m_LightDiffuseUniform{"u_lightDiffuse",""};
		
		int m_ShadowMapSamplerLocation = -1;
		std::unordered_map<shader_uniform_type, std::shared_ptr<texture>> m_DefaultTetxures;

		const std::set<int> m_AssignedTextureLocations;

        void applyMaterialProperties();

        std::shared_ptr<texture> getTextureForSampler(const std::string& samplerName,
                                                      shader_uniform_type type, const std::vector<texture_binding>& textures);
    
    public:
        explicit material(const res::material_info& info, std::vector<texture_binding>& textures, std::shared_ptr<shader_program>& program, std::unordered_map<shader_uniform_type, std::shared_ptr<texture>>& defaultTextures);
        void setMVP( glm::mat4& matrix);
        void setModelView( glm::mat4& matrix);
        void setProjection( glm::mat4& projection);
        void setModel( glm::mat4& model);
        void bind();
        int getUniformLocation(std::string name) const;
        int getAttributeLocation(std::string name) const;
        void setLight(const light_directional& light);
		void setPointLights(std::vector<light_point>& lights);
		void bindTextures(const std::vector<texture_binding>& textures);

		void setShadowMapSlot(int slot);
		void setLightViewProjection(glm::mat4& matrix);

		res::material_info& getInfo();
    };
}


#endif 
