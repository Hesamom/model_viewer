#ifndef MESH_H
#define MESH_H

namespace modelViewer::res {
    class mesh_asset;
}

namespace modelViewer::render
{
    class material;

    class mesh
    {
        public:
        virtual void bindAttributes(const material &material) = 0;
        virtual void bind()  = 0;
        virtual void draw()  = 0;
        virtual void drawLines()  = 0;
        virtual std::shared_ptr<res::mesh_asset> getAsset()  = 0;

        virtual ~mesh() = default;
    };
}
#endif //MESH_H
