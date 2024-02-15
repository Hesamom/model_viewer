
#ifndef RENDERABLE_H
#define RENDERABLE_H


#include "glm/fwd.hpp"
#include "object_renderer.h"

namespace modelViewer::render
{
	class renderable {
	public:
		virtual void render(glm::mat4 view, glm::mat4 projection) = 0;
	};
}

#endif //RENDERABLE_H
