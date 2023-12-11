
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/fwd.hpp>
#include <glm/detail/type_vec3.hpp>
#include "frustum_perspective.h"

namespace modelViewer::render
{
	class camera {
	private:
		glm::vec3 m_Position{0.0f,4.0f,10.0f};
		frustum_perspective m_Frustum {45,0.1f,500};
		int m_ViewportWidth = 0;
		int m_ViewportHeight = 0;
	public:
		glm::vec3  getPosition() const;
	 	void setPosition(glm::vec3& position);
		glm::mat4 getView() const;
		glm::mat4 getProjection() const;
		frustum_perspective& getFrustum();
		void getViewport(int& width, int& height) const;
		void setViewPort(int width, int height);
	};
}


#endif //CAMERA_H
