
#include "uniform_buffer_dx.h"

void modelViewer::render::dx::uniform_buffer_dx::setData(void* data, int offset, int size)
{
    if (offset + size > m_Size)
    {
        throw std::runtime_error("out of range args");
    }
    
    m_Buffer->set(offset, data, size);
}

int modelViewer::render::dx::uniform_buffer_dx::getSize()
{
    return m_Size;
}

modelViewer::render::dx::uniform_buffer_dx::uniform_buffer_dx(ID3D12Device& device, int size, const char* name)
{
    m_Size = size;
    m_Buffer = std::make_unique<buffer_constant_generic_dx>(device,size, name);
}

D3D12_CONSTANT_BUFFER_VIEW_DESC modelViewer::render::dx::uniform_buffer_dx::getView()
{
    return m_Buffer->getView();
}
