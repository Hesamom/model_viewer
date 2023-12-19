
#ifndef MODEL_VIEWER_LIGHT_DIRECTIONAL_H
#define MODEL_VIEWER_LIGHT_DIRECTIONAL_H

#include <glm/vec3.hpp>

namespace modelViewer::render {
    class light_directional {
        
    private:
      
    public:
		glm::vec3 position {50};
		glm::vec3 ambient{0.0f};
		glm::vec3 diffuse{0.0f};
    };
}

#endif //MODEL_VIEWER_LIGHT_DIRECTIONAL_H
