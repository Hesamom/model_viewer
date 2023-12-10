#include "frustum_perspective.h"


frustum_perspective::frustum_perspective(float fov, float near, float far)
{
	setFieldOfView(fov);
	setPlanes(near, far);
}

void frustum_perspective::setPlanes(float near, float far)
{
	if (near > far)
	{
		throw std::invalid_argument("near plan should be less than far");
	}
	
	m_NearPlane = near;
	m_FarPlane = far;
}

void frustum_perspective::getPlanes(float& near, float& far) const
{
	near = m_NearPlane;
	far = m_FarPlane;
}

void frustum_perspective::setFieldOfView(float fov)
{
	if (fov < 0)
	{
		throw std::invalid_argument("field of view should be more than zero");
	}
	m_FieldOfView = fov;
}

float frustum_perspective::getFieldOfView() const
{
	return m_FieldOfView;
}
