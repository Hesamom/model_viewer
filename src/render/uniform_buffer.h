#ifndef CONSTANT_BUFFER_H
#define CONSTANT_BUFFER_H


namespace modelViewer::render
{
    class uniform_buffer
    {
    public:
        virtual void setData(void* data, int offset, int size) = 0;
        virtual ~uniform_buffer() = default;
        virtual int getSize()   = 0;
    };
}


#endif //CONSTANT_BUFFER_H
