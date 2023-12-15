#ifndef MODEL_VIEWER_MATERIAL_H
#define MODEL_VIEWER_MATERIAL_H
#include "../resource/model_info.h"
#include "light_directional.h"
#include "texture.h"
#include "shader_uniform.h"
#include "light_point.h"


namespace modelViewer::render
{
    class texture_2D;
    class shader_program;
    class material
    {
    private:
        modelViewer::res::material_info m_Info;
        std::vector<std::shared_ptr<texture>> m_Textures;
        std::shared_ptr<shader_program> m_Program;
        
        const std::string  m_DiffuseSampler = "u_diffuseSampler";
        const std::string  m_NormalSampler = "u_normalSampler";
        const std::string  m_SkyboxSampler = "u_skybox";
		const std::string  m_ShadowSampler = "u_shadowSampler";

        //TODO consider using a uniform block
		shader_uniform<glm::mat4> m_MVPUniform{"m_MVP", ""};
		shader_uniform<glm::mat4> m_ModelViewUniform{"m_MV", ""};
		shader_uniform<glm::mat4> m_ModelUniform{"m_Model", ""};
		shader_uniform<glm::mat4> m_ProjectionUniform{"m_Projection",""};
		
		shader_uniform<glm::mat4> m_LightViewProjectionUniform{"m_LightViewProjection",""};
		shader_uniform<glm::vec3> m_LightPosUniform{"u_light_pos",""};
		shader_uniform<glm::vec3> m_LightAmbientUniform{"u_lightAmbient",""};
		shader_uniform<glm::vec3> m_LightDiffuseUniform{"u_lightDiffuse",""};
		
		int m_ShadowMapSamplerLocation = -1;
		

		const std::set<int> m_AssignedTextureLocations;

        void applyMaterialProperties();
        std::string  getSamplerName(modelViewer::res::texture_asset_type type);
    public:
        explicit material(const modelViewer::res::material_info& info, std::vector<std::shared_ptr<texture>>& textures, std::shared_ptr<shader_program>& program);
        void setMVP( glm::mat4& matrix);
        void setModelView( glm::mat4& matrix);
        void setProjection( glm::mat4& projection);
        void setModel( glm::mat4& model);
        void bind();
        int getUniformLocation(std::string name) const;
        int getAttributeLocation(std::string name) const;
        void setLight(const light_directional& light);
		void setPointLights(std::vector<light_point>& lights);
		void bindTextures(std::vector<std::shared_ptr<texture>>& textures);


		void setShadowMapSlot(int slot);
		void setLightViewProjection(glm::mat4& matrix);

		modelViewer::res::material_info& getInfo();
    };
}


#endif 
