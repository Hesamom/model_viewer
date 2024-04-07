#ifndef MESH_H
#define MESH_H

#include "shader_program.h"

namespace modelViewer::res {
    class mesh_asset;
}

namespace modelViewer::render
{
    class material;

    class mesh
    {
        public:
		virtual ~mesh() = default;
		virtual void bindLayout(const std::shared_ptr<shader_program>& program) = 0;
        virtual void bind()  = 0;
        virtual void drawTriangles()  = 0;
        virtual void drawLines()  = 0;
        virtual std::shared_ptr<res::mesh_asset> getAsset()  = 0;
    };
}
#endif //MESH_H
