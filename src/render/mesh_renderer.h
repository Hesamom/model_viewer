
#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "mesh.h"
#include "material.h"

namespace modelViewer::render
{
	class mesh_renderer {
	
	public:
		mesh_renderer(std::shared_ptr<material>& material, std::shared_ptr<mesh>& mesh, common::transform& transform, std::string& name);
		void render(glm::mat4 view, glm::mat4 projection);
		void renderShadow() const;
		std::shared_ptr<material> getMaterial();
		bool getReceiveShadows() const;
		void setReceiveShadows(bool enabled);

		bool getCastReflection() const;
		reflection_mode getReflectionMode() const;
		void setCastReflection(bool enabled);
		void setReflectionMode(reflection_mode mode);
		const std::string&  getName() const;

	private:
		common::transform& m_Transform;
		std::shared_ptr <material> m_Material;
		std::shared_ptr <mesh> m_Mesh;
		bool m_ReceivesShadow = true;
		std::string m_Name;
		reflection_mode m_ReflectionMode;
		bool m_CastReflection;
	};
}
#endif //MESH_RENDERER_H
