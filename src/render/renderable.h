
#ifndef RENDERABLE_H
#define RENDERABLE_H


#include "glm/fwd.hpp"
#include "render_object.h"

namespace modelViewer::render
{

	class renderable {
	public:
		virtual void render(glm::mat4 view, glm::mat4 projection, render_mode mode) = 0;
	};
}

#endif //RENDERABLE_H
