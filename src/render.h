#ifndef MODEL_VIEWER_RENDER_H
#define MODEL_VIEWER_RENDER_H

namespace modelViewer::render {
    class render {
    private:
        renderScene m_RenderScene;

    public:
        //TODO: get renderScene as shared pooniter
        render(renderScene renderScene);
        void draw();
    };
}

#endif
