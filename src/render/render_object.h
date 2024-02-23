#ifndef MODEL_VIEWER_RENDER_OBJECT_H
#define MODEL_VIEWER_RENDER_OBJECT_H

#include "glm/glm.hpp"
#include "../common/transform.h"
#include "../common/aabb.h"
#include "mesh.h"
#include "material.h"
#include "renderable.h"

namespace modelViewer::render
{
    enum class render_mode
    {
        triangles,
        lines
    };
    
    class render_object : public renderable {
        
    private:
        common::transform m_Transform{};
    	common::aabb m_BaseBoundingBox;
        common::aabb m_BoundingBox;
        std::shared_ptr<material> m_Material;
        std::shared_ptr<mesh> m_Mesh;
        std::string m_Name;
		render_mode m_Mode = render_mode::triangles;
    	
		bool m_CastShadows = true;
		bool m_ReceiveShadows = true;
    	bool m_IsReflective = false;
        
    public:
        render_object(std::shared_ptr<material>& material , std::shared_ptr<mesh>& mesh, const std::string& name);
        common::transform& getTransform();
        void setTransform(const common::transform& t);
        std::shared_ptr<material>& getMaterial();
        
		void setRenderMode(render_mode mode);
        void setLight(const light_directional& light);
        void render(glm::mat4 view, glm::mat4 projection) override;
		void renderShadow();
		
		void setCastShadow(bool enabled);
		void setReceiveShadows(bool enabled);
    	void setReflective(bool enabled);
    	void computeBoundingBox();
    	common::aabb getBoundingBox();
		bool getCastShadows();
		bool getReceiveShadows();
    	bool getReflective();
    	std::string getName();
    };
}
#endif
