
#ifndef MODEL_VIEWER_MESHASSET_H
#define MODEL_VIEWER_MESHASSET_H

#include <glm/vec4.hpp>
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "../common/aabb.h"

namespace modelViewer::res {
    
    class mesh_asset {
		
	private:
		int m_SubMeshIndex = -1;
		
    public:
        std::shared_ptr<std::vector<glm::vec3>> positions;
        std::shared_ptr<std::vector<glm::vec4>> colors;
        
        std::shared_ptr<std::vector<unsigned int>> indices;

        std::shared_ptr<std::vector<glm::vec3>> normals;
        std::shared_ptr<std::vector<glm::vec3>> tangents;
        std::shared_ptr<std::vector<glm::vec2>> UV0;

		explicit mesh_asset(int subMeshIndex = 0)
		{
			if (subMeshIndex < 0)
			{
				throw std::range_error("mesh index must be more or equal to zero");
			}
			
			m_SubMeshIndex = subMeshIndex;
		}
		
		int getSubMeshIndex()
		{
			return m_SubMeshIndex;
		}
		
        common::aabb getBoundingBox() const {
            return common::aabb{*positions};
        }
        std::string name;
    };
}

#endif
