#ifndef RENDERER_FORWARD_H
#define RENDERER_FORWARD_H

#include "render_scene.h"
#include "camera.h"
#include "framebuffer.h"
#include "object_factory.h"
#include "texture_cube.h"

namespace modelViewer::render
{
	enum class clear_mode
	{
		color ,
		skybox
	};
	

	class renderer_forward {
		
	private:
		glm::vec4 m_ClearFlag;
		glm::vec4 m_ReflectionClearFlag;
		clear_mode m_ClearMode = clear_mode::color;
		clear_mode m_ReflectionClearMode = clear_mode::skybox;
		
		const unsigned int SHADOW_DIR_WIDTH = 2048;
		const unsigned int SHADOW_DIR_HEIGHT = SHADOW_DIR_WIDTH;
		const unsigned int REFLECTION_SIZE = 128;

		const unsigned int SUPPORTED_SPOT_LIGHTS = 4;
		const unsigned int SHADOW_SPOT_WIDTH = 1024;
		const unsigned int SHADOW_SPOT_HEIGHT = SHADOW_SPOT_WIDTH;

		//TODO need to get max supported texture units instead of assuming we have 32 units available 
		const int maxTextureSlot = 31;
		const int emptyShadowmapSlot = maxTextureSlot;
		const int shadowmapDirSlot = maxTextureSlot - 1;
		const int shadowmapSpotSlot = maxTextureSlot - 2;


		const int emptyReflectionMapSlot = maxTextureSlot - 3;
		const int reflectionMapSlot = maxTextureSlot - 4;
		const int skyboxReflectionMapSlot = maxTextureSlot - 5;
		
		const std::string m_EmptyShadowmapTexture = modelViewer::res::literals::textures::default_shadowmap;
		const std::string m_EmptyReflectionTexturePath = modelViewer::res::literals::textures::default_white;
		
		const std::string m_DepthShaderVert = modelViewer::res::literals::shaders::shadow_vert;
		const std::string m_DepthShaderFrag = modelViewer::res::literals::shaders::shadow_frag;
		const std::string m_MVPUniformName = "u_MVP";

		framebuffer m_shadowBuffer;
		framebuffer m_ReflectionBuffer;
		std::shared_ptr<texture> m_SkyboxCubeMap;
		
		std::unique_ptr<shader_program> m_shadowProgram;
		std::shared_ptr<object_renderer> m_Skybox;
		
		std::shared_ptr<texture> m_EmptyShadowmap;
		std::shared_ptr<texture_cube> m_EmptyReflectionMap;
		
		int m_MVPLocation = -1;
		glm::mat4 m_LightViewProjection;
		glm::vec3  m_ReflectionPosition;

		void renderSpotShadows(render_scene& scene);
		void renderDirectionalShadows(render_scene& scene);
		void renderShadows(render_scene& scene);
		static bool requiresReflectionMapUpdate(render_scene& scene, camera& camera);
		void renderReflectionMap(render_scene& scene, camera& camera);

		std::vector<std::shared_ptr<mesh_renderer>> getSortedObjects(render_scene& scene, bool includeSkybox);
		void initReflectionMap(object_factory& objectFactory);
		void initShadowmap(object_factory& objectFactory, res::shader_loader& shaderLoader);
		void initSkybox(object_factory& objectFactory);
		void renderObjects(render_scene& scene, camera& camera, bool shadowsEnabled, bool reflectionEnabled);
		
	public:
		
		void render(render_scene& scene, camera& camera, bool shadowEnabled, bool reflectionEnabled);
		
		void init(object_factory& objectFactory);
		void setClearFlag(glm::vec4 color);
		void setReflectionPosition(const glm::vec3& pos);
		glm::vec3& getReflectionPosition();
		void setClearMode(clear_mode mode);
		void setReflectionClearMode(clear_mode mode);
		void setReflectionClearFlag(const glm::vec4& color);
	
	};
}


#endif //RENDERER_FORWARD_H
