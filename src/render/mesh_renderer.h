﻿
#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "mesh.h"
#include "material.h"

namespace modelViewer::render
{
	enum class render_mode
	{
		triangles,
		lines
	};

	
	class mesh_renderer {
	private:
		common::transform& m_Transform;
		std::shared_ptr <material> m_Material;
		std::shared_ptr <mesh> m_Mesh;
		render_mode m_Mode = render_mode::triangles;
		bool m_ReceivesShadow = true;
		bool m_CastReflection = true;
		reflection_mode m_ReflectionMode = reflection_mode::disabled;
		std::string m_Name;
		
	public:
		mesh_renderer(std::shared_ptr<material>& material, std::shared_ptr<mesh>& mesh, common::transform& transform, std::string& name);
		void setRenderMode(render_mode mode);
		void render(glm::mat4 view, glm::mat4 projection);
		void renderShadow();
		std::shared_ptr<material> getMaterial();
		bool getReceiveShadows() const;
		void setReceiveShadows(bool enabled);

		bool getCastReflection() const;
		reflection_mode getReflectionMode() const;
		void setCastReflection(bool enabled);
		void setReflectionMode(reflection_mode mode);
		const std::string&  getName() const;
	};
}
#endif //MESH_RENDERER_H
