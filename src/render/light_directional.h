
#ifndef MODEL_VIEWER_LIGHT_DIRECTIONAL_H
#define MODEL_VIEWER_LIGHT_DIRECTIONAL_H

#include <glm/vec3.hpp>

namespace modelViewer::render {
    class light_directional {
      
    public:
		glm::vec3 direction {1};
    	glm::vec3 rotation {1};
		glm::vec3 ambient{0.7f};
		glm::vec3 diffuse{0.5f};
    };
}

#endif //MODEL_VIEWER_LIGHT_DIRECTIONAL_H
