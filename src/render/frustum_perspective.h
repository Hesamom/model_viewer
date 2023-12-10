
#ifndef FRUSTUM_PERSPECTIVE_H
#define FRUSTUM_PERSPECTIVE_H

#include <glm/detail/type_vec3.hpp>
#include <glm/fwd.hpp>

class frustum_perspective
{
private:
	float m_FieldOfView = 60;
	float m_NearPlane = 0;
	float m_FarPlane = 100;
public:
	frustum_perspective(float fov, float near, float far);
	void setPlanes(float near, float far);
	void getPlanes(float& near, float& far) const;
	void setFieldOfView(float fov);
	float getFieldOfView() const;
};

#endif //FRUSTUM_PERSPECTIVE_H
