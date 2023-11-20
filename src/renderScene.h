#ifndef MODEL_VIEWER_RENDERSCENE_H
#define MODEL_VIEWER_RENDERSCENE_H

namespace modelViewer::render {
    class renderScene {
    private:
        std::vector<renderObject> m_RenderObjects;
    public:
        //TODO: shared pointer of renderObjects, make vector reference
        renderScene(std::vector<renderObject> renderObjects);
        //TODO:implement begion and end function instead of GetObjects
        std::vector<renderObject> GetObjects();
    };
}
#endif
