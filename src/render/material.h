#ifndef MODEL_VIEWER_MATERIAL_H
#define MODEL_VIEWER_MATERIAL_H
#include <complex.h>
#include <set>
#include <unordered_map>
#include <vector>

#include "../resource/model_info.h"
#include "light_directional.h"
#include "texture.h"
#include "shader_uniform.h"
#include "light_point.h"
#include "light_spot.h"
#include "shader_program.h"


namespace modelViewer::render
{
    class texture_2D;
    class shader_program;
    class material
    {
    private:
        res::material_asset m_Info;
        std::vector<std::shared_ptr<texture>> m_ActiveTextures;
        std::shared_ptr<shader_program> m_Program;
    	
        //TODO consider using a uniform block
    	const std::string  m_ShadowSampler = "u_shadowSampler";
    	const std::string  m_SpotShadowSampler = "u_spotShadowSamplers";
    	
		shader_uniform<glm::mat4> m_MVPUniform{"m_MVP", ""};
		shader_uniform<glm::mat4> m_ModelViewUniform{"m_MV", ""};
		shader_uniform<glm::mat4> m_ModelUniform{"m_Model", ""};
		shader_uniform<glm::mat4> m_ProjectionUniform{"m_Projection",""};
		
		shader_uniform<glm::mat4> m_LightViewProjectionUniform{"m_LightViewProjection",""};
		shader_uniform<glm::vec3> m_LightDirUniform{"u_light_dir",""};
		shader_uniform<glm::vec3> m_LightAmbientUniform{"u_lightAmbient",""};
		shader_uniform<glm::vec3> m_LightDiffuseUniform{"u_lightDiffuse",""};
		shader_uniform<glm::vec3> m_CameraPositionUniform{"u_camera_pos",""};
		
		int m_ShadowMapSamplerLocation = -1;
    	int m_SpotShadowMapSamplerLocation = -1;
		std::map<shader_uniform_texture_pair, std::shared_ptr<texture>> m_DefaultTextures;

		const std::set<int> m_AssignedTextureLocations;

        void applyMaterialProperties();

        std::shared_ptr<texture> getTextureForSampler(const shader_uniform_info& info, const std::vector<texture_binding>& textures);

    	int getMaxSupportedTextureUnits();
    public:
        explicit material(const res::material_asset& info, std::vector<texture_binding>& textures, std::shared_ptr<shader_program>& program, std::map<shader_uniform_texture_pair, std::shared_ptr<texture>>& defaultTextures);
        void setMVP( glm::mat4& matrix);
        void setModelView( glm::mat4& matrix);
        void setProjection( glm::mat4& projection);
        void setModel( glm::mat4& model);
        void bind();
        int getUniformLocation(std::string name) const;
        int getAttributeLocation(std::string name) const;
        void setDirectionalLight(const light_directional& light);
		void setPointLights(std::vector<light_point>& lights);
		void bindTextures(const std::vector<texture_binding>& textures);

		void setShadowMapSlot(int slot) const;
        void setSpotShadowMapSlot(int slot) const;

        void setLightViewProjection(glm::mat4& matrix);

		res::material_asset& getInfo();

        void setSpotLights(std::vector<light_spot>& lights);

		void setCameraPosition(glm::vec3 position);
	};
}


#endif 
