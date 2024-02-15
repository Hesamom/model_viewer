#ifndef RENDERER_FORWARD_H
#define RENDERER_FORWARD_H

#include "render_scene.h"
#include "camera.h"
#include "framebuffer.h"
#include "object_factory.h"

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
		clear_mode m_ClearMode = clear_mode::color;
		
		const unsigned int SHADOW_DIR_WIDTH = 2048;
		const unsigned int SHADOW_DIR_HEIGHT = SHADOW_DIR_WIDTH;

		const unsigned int SUPPORTTED_SPOT_LIGHTS = 4;
		const unsigned int SHADOW_SPOT_WIDTH = 1024;
		const unsigned int SHADOW_SPOT_HEIGHT = SHADOW_SPOT_WIDTH;

		//TODO need to get max supported texture units instead of assuming we have 32 units available 
		const int shadowmapDirSlot = 29;
		const int shadowmapSpotSlot = shadowmapDirSlot + 1;
		const int emptyShadowmapSlot = shadowmapSpotSlot + 1;

		const std::string m_EmptyShadowmapTexture = "res/textures/default/white.png";
		
		const std::string m_DepthShaderVert = "res/shaders/sample/simple_depth_vert.glsl";
		const std::string m_DepthShaderFrag = "res/shaders/sample/simple_depth_frag.glsl";
		const std::string m_MVPUniformName = "u_MVP";

		framebuffer m_shadowBuffer;
		std::unique_ptr<shader_program> m_shadowProgram;
		std::shared_ptr<object_renderer> m_Skybox;
		std::shared_ptr<texture> m_EmptyShadowmap;
		
		int m_MVPLocation = -1;
		glm::mat4 m_LightViewProjection;

		void renderSpotShadows(render_scene& scene);
		void renderDirectionalShadows(render_scene& scene);
		void renderShadows(render_scene& scene);
		
		void renderObjects(render_scene& scene, camera& camera, bool b);

		std::vector<std::shared_ptr<mesh_renderer>> getSortedObjects(render_scene& scene);
		
	public:
		void render(render_scene& scene, camera& camera, bool shadowEnabled);
		
		void setClearFlag(glm::vec4 color);
		void init(object_factory& objectFactory);

		void createSkybox(object_factory& objectFactory);

		void setClearMode(clear_mode mode);

		void initShadowmap(object_factory& objectFactory, res::shader_loader& shaderLoader);
	};
}


#endif //RENDERER_FORWARD_H
