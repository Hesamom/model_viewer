﻿
#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "gl/mesh_gl.h"
#include "material.h"

namespace modelViewer::render
{
	enum class render_mode
	{
		triangles,
		lines
	};
	
	class mesh_renderer {
	
	public:
		mesh_renderer(std::shared_ptr<material>& material, std::shared_ptr<mesh>& mesh, common::transform& transform, std::string& name);
		void setRenderMode(render_mode mode);
		void render(glm::mat4 view, glm::mat4 projection);
		void renderShadow() const;
		std::shared_ptr<material> getMaterial();
		bool getReceiveShadows() const;
		void setReceiveShadows(bool enabled);
		const std::string&  getName() const;

	private:
		common::transform& m_Transform;
		std::shared_ptr <material> m_Material;
		std::shared_ptr <mesh> m_Mesh;
		render_mode m_Mode = render_mode::triangles;
		bool m_ReceivesShadow = true;
		std::string m_Name;
	};
}
#endif //MESH_RENDERER_H
