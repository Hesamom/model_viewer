#ifndef MODEL_VIEWER_MATERIAL_H
#define MODEL_VIEWER_MATERIAL_H

#include "gfx_device.h"
#include "../resource/model_info.h"
#include "light_directional.h"
#include "shader_uniform.h"
#include "light_point.h"
#include "light_spot.h"
#include "texture.h"


namespace modelViewer::render
{
	enum class reflection_mode
	{
		disabled,
		environment,
		skybox
	};
	
    class texture_2D_gl;
    class shader_program_gl;
    class material
    {
    private:
        res::material_asset m_Info;
        std::shared_ptr<shader_program> m_Program;
    	
        //TODO consider using a uniform block
    	const std::string  m_ShadowSampler = "u_shadowSampler";
		const std::string  m_ReflectionSampler = "u_reflectionSampler";
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
		
		std::map<shader_uniform_texture_pair, std::shared_ptr<texture>> m_DefaultTextures;

		const std::set<int> m_AssignedTextureLocations;
    	std::shared_ptr<gfx_device> m_Device;
        std::shared_ptr<uniform_buffer> m_MaterialBuffer;

        void applyMaterialProperties();

        std::shared_ptr<texture> getTextureForSampler(const shader_texture_slot& slot, const std::vector<texture_binding>& bindings);
    	int getMaxSupportedTextureUnits();
		static shader_texture_usage getUsageByName(const std::string& basicString);
		
    public:

    	material(std::shared_ptr<gfx_device>& device, const res::material_asset& info, std::vector<texture_binding>& textures, std::shared_ptr<shader_program>& program, std::map<shader_uniform_texture_pair, std::shared_ptr<texture>>& defaultTextures);
		
		std::shared_ptr<shader_program> getShaderProgram();
        void setMVP( glm::mat4& matrix);
        void setModelView( glm::mat4& matrix);
        void setProjection( glm::mat4& projection);
        void setModel( glm::mat4& model);
        void bind();
        void setDirectionalLight(const light_directional& light);
		void setPointLights(std::vector<light_point>& lights);
		void bindAllTextures(const std::vector<texture_binding>& textures);

		void setDirectionalShadowMap(std::shared_ptr<texture>& texture);
        void setSpotShadowMap(std::shared_ptr<texture>& texture);
		void setReflectionMap(std::shared_ptr<texture>& texture);

        void setLightViewProjection(glm::mat4& matrix);

		res::material_asset& getInfo();

        void setSpotLights(std::vector<light_spot>& lights);

		void setCameraPosition(glm::vec3 position);
		bool isReflective() const;

		void bindTexture(texture_binding& binding);

		void bindProgram();

		bool m_ProgramBound = false;
		std::vector<texture_binding> m_ToBindTextures;
	};
}


#endif 
