#include "aabb.h"

using namespace modelViewer::common;



aabb::aabb(std::vector<glm::vec3>& positions) {

    glm::vec3 min{std::numeric_limits<float>::max()};
    glm::vec3 max{std::numeric_limits<float>::min()};

    for (const auto & position : positions) {
        min.x = std::min(position.x, min.x);
        min.y = std::min(position.y, min.y);
        min.z = std::min(position.z, min.z);
        
        max.x = std::max(position.x, max.x);
        max.y = std::max(position.y, max.y);
        max.z = std::max(position.z, max.z);
    }

    m_Min = min;
    m_Max = max;
    m_Size = m_Max - m_Min;
}

aabb::aabb(glm::vec3 min, glm::vec3 max) {
    m_Min = min;
    m_Max = max;

    m_Size = m_Max - m_Min;
}

aabb::aabb() {
}

glm::vec3 aabb::getMin() {
    return m_Min;
}

glm::vec3 aabb::getMax() {
    return m_Max;
}

glm::vec3 aabb::getSize() {
    return m_Size;
}
