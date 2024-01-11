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
		
		const unsigned int SHADOW_WIDTH = 1024;
		const unsigned int SHADOW_HEIGHT = 1024;

		const int shadowmapSlot = 30;
		const int emptyShadowmapSlot = shadowmapSlot + 1;

		const std::string m_EmptyShadowmapTexture = "res/textures/default/white.png";
		
		const std::string m_DepthShaderVert = "res/shaders/sample/simple_depth_vert.glsl";
		const std::string m_DepthShaderFrag = "res/shaders/sample/simple_depth_frag.glsl";
		const std::string m_MVPUniformName = "u_MVP";

		framebuffer m_shadowBuffer;
		std::unique_ptr<shader_program> m_shadowProgram;
		std::shared_ptr<render_object> m_Skybox;
		std::shared_ptr<texture> m_EmptyShadowmap;
		
		int m_MVPLocation = -1;
		glm::mat4 m_LightViewProjection;

		void renderSpotShadows(render_scene& scene);
		void renderDirectionalShadows(render_scene& scene);
		void renderShadows(render_scene& scene, camera& camera);
		
		void renderObjects(render_scene& scene, camera& camera, bool b);
	public:
		void render(render_scene& scene, camera& camera, bool shadowEnabled);
		
		void setClearFlag(glm::vec4 color);
		void init(modelViewer::render::object_factory& objectFactory);

		void createSkybox(object_factory& objectFactory);

		void setClearMode(clear_mode mode);

		std::vector<std::shared_ptr<render_object>> getSortedObjects(render_scene& scene);
	};
}


#endif //RENDERER_FORWARD_H
