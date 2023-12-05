
#ifndef MODEL_VIEWER_LIGHT_DIRECTIONAL_H
#define MODEL_VIEWER_LIGHT_DIRECTIONAL_H

#include <glm/vec3.hpp>

namespace modelViewer::render {
    class light_directional {
        
    private:
        glm::vec3 m_Position {50};
        glm::vec3 m_Color{1};
    public:
        glm::vec3 getPosition() const;
        void setPosition(glm::vec3 position);
        
        glm::vec3 getColor() const;
        void setColor(glm::vec3 color);
    };
}

#endif //MODEL_VIEWER_LIGHT_DIRECTIONAL_H
