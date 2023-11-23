
#ifndef MODEL_VIEWER_VERTEX_ARRAY_H
#define MODEL_VIEWER_VERTEX_ARRAY_H


namespace modelViewer::render {
    
    class vertex_array
    {
    private:
        unsigned int m_arrayId;
    public:
        vertex_array();
        void bind();
    };
}

#endif
