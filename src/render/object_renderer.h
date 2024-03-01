#ifndef MODEL_VIEWER_RENDER_OBJECT_H
#define MODEL_VIEWER_RENDER_OBJECT_H

#include "glm/glm.hpp"
#include "../common/transform.h"
#include "../common/aabb.h"
#include "mesh.h"
#include "material.h"
#include "renderable.h"
#include "mesh_renderer.h"

namespace modelViewer::render
{
   
    class object_renderer : public renderable {
    
    public:
        object_renderer(std::vector<std::shared_ptr<material>>& materials,
			std::vector<std::shared_ptr<mesh>>& meshes,
			const std::string& name);

		object_renderer(std::shared_ptr<material>& material, std::shared_ptr<mesh>& mesh, const std::string& name);

		std::vector<std::shared_ptr<mesh_renderer>>& getRenderers();
        common::transform& getTransform();
        void setTransform(const common::transform& t);
		
        void render(glm::mat4 view, glm::mat4 projection) override;
		void setRenderMode(render_mode mode);
		void setCastShadow(bool enabled);
		void setReceiveShadows(bool enabled);
		bool getReceiveShadows();
		bool getCastShadows();
		
		bool getCastReflection() const;
		reflection_mode getReflectionMode() const;
		void setCastReflection(bool enabled);
		void setReflectionMode(reflection_mode mode);
		
    	void computeBoundingBox();
    	common::aabb getBoundingBox();
		
    	std::string getName();
    	
    private:
    	common::transform m_Transform{};
    	common::aabb m_BaseBoundingBox;
    	common::aabb m_BoundingBox;
    	std::string m_Name;
    	std::vector<std::shared_ptr<mesh_renderer>> m_MeshRenders;
		
    	bool m_CastShadows = true;
    	bool m_ReceiveShadows = true;
    };
}
#endif
