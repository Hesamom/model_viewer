
#ifndef AABB_H
#define AABB_H
#include <glm/vec3.hpp>


namespace modelViewer::common {
    class aabb {
    private:
        glm::vec3 m_Min{0};
        glm::vec3 m_Max{0};
        glm::vec3 m_Size{0};
    public:
        explicit aabb(std::vector<glm::vec3>& positions);
        aabb(glm::vec3 min, glm::vec3 max);
        aabb();
        glm::vec3 getMin();
        glm::vec3 getMax();
        glm::vec3 getSize();
    };
}



#endif //AABB_H
